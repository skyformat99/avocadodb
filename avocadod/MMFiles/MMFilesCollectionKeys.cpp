////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#include "MMFilesCollectionKeys.h"
#include "Basics/StaticStrings.h"
#include "Basics/StringRef.h"
#include "MMFiles/MMFilesCollection.h"
#include "MMFiles/MMFilesDitch.h"
#include "MMFiles/MMFilesLogfileManager.h" 
#include "MMFiles/MMFilesEngine.h"
#include "StorageEngine/EngineSelectorFeature.h"
#include "Transaction/Helpers.h"
#include "Utils/CollectionGuard.h"
#include "Utils/SingleCollectionTransaction.h"
#include "Transaction/StandaloneContext.h"
#include "VocBase/LogicalCollection.h"
#include "VocBase/vocbase.h"

#include <velocypack/Builder.h>
#include <velocypack/Iterator.h>
#include <velocypack/velocypack-aliases.h>

using namespace avocadodb;

MMFilesCollectionKeys::MMFilesCollectionKeys(TRI_vocbase_t* vocbase, std::string const& name,
                                             TRI_voc_tick_t blockerId, double ttl)
    : CollectionKeys(vocbase, name, ttl),
      _ditch(nullptr),
      _resolver(vocbase),
      _blockerId(blockerId) {
  TRI_ASSERT(_blockerId > 0);

  // prevent the collection from being unloaded while the export is ongoing
  // this may throw
  _guard.reset(new avocadodb::CollectionGuard(vocbase, _name.c_str(), false));

  _collection = _guard->collection();
  TRI_ASSERT(_collection != nullptr);
}

MMFilesCollectionKeys::~MMFilesCollectionKeys() {
  // remove compaction blocker
  MMFilesEngine* engine = static_cast<MMFilesEngine*>(EngineSelectorFeature::ENGINE);
  engine->removeCompactionBlocker(_vocbase, _blockerId);

  if (_ditch != nullptr) {
    _ditch->ditches()->freeMMFilesDocumentDitch(_ditch, false);
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief initially creates the list of keys
////////////////////////////////////////////////////////////////////////////////

void MMFilesCollectionKeys::create(TRI_voc_tick_t maxTick) {
  MMFilesLogfileManager::instance()->waitForCollectorQueue(
      _collection->cid(), 30.0);
  
  MMFilesEngine* engine = static_cast<MMFilesEngine*>(EngineSelectorFeature::ENGINE);
  engine->preventCompaction(_collection->vocbase(), [this](TRI_vocbase_t* vocbase) {
    // create a ditch under the compaction lock
    _ditch = avocadodb::MMFilesCollection::toMMFilesCollection(_collection)
                 ->ditches()
                 ->createMMFilesDocumentDitch(false, __FILE__, __LINE__);
  });

  // now we either have a ditch or not
  if (_ditch == nullptr) {
    THROW_AVOCADO_EXCEPTION(TRI_ERROR_OUT_OF_MEMORY);
  }

  _vpack.reserve(16384);

  // copy all document tokens into the result under the read-lock
  {
    SingleCollectionTransaction trx(
        transaction::StandaloneContext::Create(_collection->vocbase()), _name,
        AccessMode::Type::READ);

    Result res = trx.begin();

    if (!res.ok()) {
      THROW_AVOCADO_EXCEPTION(res);
    }

    ManagedDocumentResult mmdr;
    MMFilesCollection *mmColl = MMFilesCollection::toMMFilesCollection(_collection);
    trx.invokeOnAllElements(
        _collection->name(), [this, &trx, &maxTick, &mmdr, &mmColl](DocumentIdentifierToken const& token) {
          if (mmColl->readDocumentConditional(&trx, token, maxTick, mmdr)) {
            _vpack.emplace_back(mmdr.vpack());
          }
          return true;
        });

    trx.finish(res);
  }

  // now sort all document tokens without the read-lock
  std::sort(_vpack.begin(), _vpack.end(),
            [](uint8_t const* lhs, uint8_t const* rhs) -> bool {
    return (StringRef(transaction::helpers::extractKeyFromDocument(VPackSlice(lhs))) < StringRef(transaction::helpers::extractKeyFromDocument(VPackSlice(rhs))));
  });
}

////////////////////////////////////////////////////////////////////////////////
/// @brief hashes a chunk of keys
////////////////////////////////////////////////////////////////////////////////

std::tuple<std::string, std::string, uint64_t> MMFilesCollectionKeys::hashChunk(
    size_t from, size_t to) const {
  if (from >= _vpack.size() || to > _vpack.size() || from >= to ||
      to == 0) {
    THROW_AVOCADO_EXCEPTION(TRI_ERROR_BAD_PARAMETER);
  }

  VPackSlice first(_vpack.at(from));
  VPackSlice last(_vpack.at(to - 1));

  TRI_ASSERT(first.isObject());
  TRI_ASSERT(last.isObject());

  uint64_t hash = 0x012345678;

  for (size_t i = from; i < to; ++i) {
    VPackSlice current(_vpack.at(i));
    TRI_ASSERT(current.isObject());

    // we can get away with the fast hash function here, as key values are 
    // restricted to strings
    hash ^= transaction::helpers::extractKeyFromDocument(current).hashString();
    hash ^= transaction::helpers::extractRevSliceFromDocument(current).hash();
  }

  return std::make_tuple(
    transaction::helpers::extractKeyFromDocument(first).copyString(),
    transaction::helpers::extractKeyFromDocument(last).copyString(),
    hash);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief dumps keys into the result
////////////////////////////////////////////////////////////////////////////////

void MMFilesCollectionKeys::dumpKeys(VPackBuilder& result, size_t chunk,
                              size_t chunkSize) const {
  size_t from = chunk * chunkSize;
  size_t to = (chunk + 1) * chunkSize;

  if (to > _vpack.size()) {
    to = _vpack.size();
  }

  if (from >= _vpack.size() || from >= to || to == 0) {
    THROW_AVOCADO_EXCEPTION(TRI_ERROR_BAD_PARAMETER);
  }
  
  for (size_t i = from; i < to; ++i) {
    VPackSlice current(_vpack.at(i));
    TRI_ASSERT(current.isObject());

    result.openArray();
    result.add(current.get(StaticStrings::KeyString));
    result.add(current.get(StaticStrings::RevString));
    result.close();
  }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief dumps documents into the result
////////////////////////////////////////////////////////////////////////////////

void MMFilesCollectionKeys::dumpDocs(avocadodb::velocypack::Builder& result, size_t chunk,
                              size_t chunkSize, VPackSlice const& ids) const {
  if (!ids.isArray()) {
    THROW_AVOCADO_EXCEPTION(TRI_ERROR_BAD_PARAMETER);
  }

  for (auto const& it : VPackArrayIterator(ids)) {
    if (!it.isNumber()) {
      THROW_AVOCADO_EXCEPTION(TRI_ERROR_BAD_PARAMETER);
    }

    size_t position = chunk * chunkSize + it.getNumber<size_t>();

    if (position >= _vpack.size()) {
      THROW_AVOCADO_EXCEPTION(TRI_ERROR_BAD_PARAMETER);
    }
    
    VPackSlice current(_vpack.at(position));
    TRI_ASSERT(current.isObject());
  
    result.add(current);
  }
}

