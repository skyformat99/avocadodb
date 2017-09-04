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

#ifndef AVOCADOD_MMFILES_MMFILES_WAL_MARKER_H
#define AVOCADOD_MMFILES_MMFILES_WAL_MARKER_H 1

#include "Basics/Common.h"
#include "Basics/encoding.h"
#include "MMFiles/MMFilesDatafile.h"
#include "MMFiles/MMFilesDatafileHelper.h"
#include "VocBase/voc-types.h"

#include <velocypack/Slice.h>

namespace avocadodb {

/// @brief abstract base class for all markers
class MMFilesWalMarker {
 private:
  MMFilesWalMarker& operator=(MMFilesWalMarker const&) = delete;
  MMFilesWalMarker(MMFilesWalMarker&&) = delete;
  MMFilesWalMarker(MMFilesWalMarker const&) = delete;
 protected:
  MMFilesWalMarker() = default;

 public:
  virtual ~MMFilesWalMarker() {}
 
  /// @brief returns the marker type 
  virtual MMFilesMarkerType type() const = 0;

  /// @brief returns the datafile id the marker comes from
  /// this should be 0 for new markers, but contain the actual
  /// datafile id for an existing marker during recovery
  virtual TRI_voc_fid_t fid() const = 0;

  /// @brief return the total size of the marker, including the header
  virtual uint32_t size() const = 0;

  /// @brief store the marker in the memory region starting at mem
  /// the region is guaranteed to be big enough to hold size() bytes
  virtual void store(char* mem) const = 0;

  /// @brief a pointer to the beginning of the VPack payload
  virtual uint8_t* vpack() const {
    // not implemented for base marker type
    TRI_ASSERT(false);
    return nullptr;
  }
};

/// @brief an envelope that contains a pointer to an existing marker
/// this type is used during recovery only, to represent existing markers
class MMFilesMarkerEnvelope : public MMFilesWalMarker {
 public:
  MMFilesMarkerEnvelope(MMFilesMarker const* other, TRI_voc_fid_t fid) 
      : _other(other),
        _fid(fid),
        _size(other->getSize()) {
    // we must always have a datafile id, and a reasonable marker size
    TRI_ASSERT(_fid > 0);
    TRI_ASSERT(_size >= sizeof(MMFilesMarker));
  }

  ~MMFilesMarkerEnvelope() = default;

  /// @brief returns the marker type 
  MMFilesMarkerType type() const override final { 
    // simply return the wrapped marker's type
    return _other->getType(); 
  }
  
  /// @brief returns the datafile id the marker comes from
  TRI_voc_fid_t fid() const override final { return _fid; }
 
  /// @brief a pointer the beginning of the VPack payload
  uint8_t* vpack() const override final { 
    return const_cast<uint8_t*>(reinterpret_cast<uint8_t const*>(_other) + MMFilesDatafileHelper::VPackOffset(type())); 
  }

  /// @brief a pointer to the beginning of the wrapped marker
  void const* mem() const {
    return static_cast<void const*>(_other);
  }

  /// @brief return the size of the wrapped marker
  uint32_t size() const override final { return _size; }

  /// @brief store the marker in the memory region starting at mem
  void store(char* mem) const override final {
    // intentionally nothing... should never be called for MarkerEnvelopes,
    // as they represent existing markers from the WAL that do not need to
    // be written again!
    TRI_ASSERT(false); 
  }

 private:
  MMFilesMarker const* _other;
  TRI_voc_fid_t _fid;
  uint32_t _size;
};

/// @brief a marker type that is used when inserting new documents,
/// updating/replacing or removing documents
class MMFilesCrudMarker : public MMFilesWalMarker {
 public:
  MMFilesCrudMarker(MMFilesMarkerType type, 
             TRI_voc_tid_t transactionId, 
             avocadodb::velocypack::Slice const& data)
    : _transactionId(transactionId),
      _data(data),
      _type(type) {}

  ~MMFilesCrudMarker() = default;

  /// @brief returns the marker type 
  MMFilesMarkerType type() const override final { return _type; }
  
  /// @brief returns the datafile id
  /// this is always 0 for this type of marker, as the marker is not
  /// yet in any datafile
  TRI_voc_fid_t fid() const override final { return 0; }
 
  /// @brief returns the marker size 
  uint32_t size() const override final { 
    return static_cast<uint32_t>(MMFilesDatafileHelper::VPackOffset(_type) + _data.byteSize());
  }

  /// @brief store the marker in the memory region starting at mem
  void store(char* mem) const override final { 
    // store transaction id
    encoding::storeNumber<decltype(_transactionId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::TransactionIdOffset(_type), _transactionId, sizeof(decltype(_transactionId)));
    // store VPack
    memcpy(mem + MMFilesDatafileHelper::VPackOffset(_type), _data.begin(), static_cast<size_t>(_data.byteSize()));
  }
  
  /// @brief a pointer the beginning of the VPack payload
  uint8_t* vpack() const override final { return const_cast<uint8_t*>(_data.begin()); }

 private:
  TRI_voc_tid_t _transactionId;
  avocadodb::velocypack::Slice _data;
  MMFilesMarkerType _type;
};

/// @brief a marker used for database-related operations
class MMFilesDatabaseMarker : public MMFilesWalMarker {
 public:
  MMFilesDatabaseMarker(MMFilesMarkerType type, 
                 TRI_voc_tick_t databaseId, 
                 avocadodb::velocypack::Slice const& data)
    : _databaseId(databaseId),
      _data(data),
      _type(type) {
    TRI_ASSERT(databaseId > 0);
  }

  ~MMFilesDatabaseMarker() = default;

  /// @brief returns the marker type 
  MMFilesMarkerType type() const override final { return _type; }

  /// @brief returns the datafile id
  /// this is always 0 for this type of marker, as the marker is not
  /// yet in any datafile
  TRI_voc_fid_t fid() const override final { return 0; }
  
  /// @brief returns the marker size 
  uint32_t size() const override final { 
    return static_cast<uint32_t>(MMFilesDatafileHelper::VPackOffset(_type) + _data.byteSize());
  }

  /// @brief store the marker in the memory region starting at mem
  void store(char* mem) const override final { 
    // store database id
    encoding::storeNumber<decltype(_databaseId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::DatabaseIdOffset(_type), _databaseId, sizeof(decltype(_databaseId)));
    // store VPack
    memcpy(mem + MMFilesDatafileHelper::VPackOffset(_type), _data.begin(), static_cast<size_t>(_data.byteSize()));
  }

 private:
  TRI_voc_tick_t _databaseId;
  avocadodb::velocypack::Slice _data;
  MMFilesMarkerType _type;
};

/// @brief a marker used for collection-related operations
class MMFilesCollectionMarker : public MMFilesWalMarker {
 public:
  MMFilesCollectionMarker(MMFilesMarkerType type, 
                   TRI_voc_tick_t databaseId, 
                   TRI_voc_cid_t collectionId, 
                   avocadodb::velocypack::Slice const& data)
    : _databaseId(databaseId),
      _collectionId(collectionId),
      _data(data.begin()),
      _type(type) {

    TRI_ASSERT(databaseId > 0);
    TRI_ASSERT(collectionId > 0);
  }

  ~MMFilesCollectionMarker() = default;

  /// @brief returns the marker type 
  MMFilesMarkerType type() const override final { return _type; }
  
  /// @brief returns the datafile id
  /// this is always 0 for this type of marker, as the marker is not
  /// yet in any datafile
  TRI_voc_fid_t fid() const override final { return 0; }
  
  /// @brief returns the marker size 
  uint32_t size() const override final { 
    return static_cast<uint32_t>(MMFilesDatafileHelper::VPackOffset(_type) + _data.byteSize());
  }

  /// @brief store the marker in the memory region starting at mem
  void store(char* mem) const override final { 
    // store database id
    encoding::storeNumber<decltype(_databaseId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::DatabaseIdOffset(_type), _databaseId, sizeof(decltype(_databaseId)));
    // store collection id
    encoding::storeNumber<decltype(_collectionId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::CollectionIdOffset(_type), _collectionId, sizeof(decltype(_collectionId)));
    // store VPack
    memcpy(mem + MMFilesDatafileHelper::VPackOffset(_type), _data.begin(), static_cast<size_t>(_data.byteSize()));
  }

 private:
  TRI_voc_tick_t _databaseId;
  TRI_voc_cid_t _collectionId;
  avocadodb::velocypack::Slice _data;
  MMFilesMarkerType _type;
};

/// @brief a marker used for view-related operations
class MMFilesViewMarker : public MMFilesWalMarker {
 public:
  MMFilesViewMarker(MMFilesMarkerType type, 
                    TRI_voc_tick_t databaseId, 
                    TRI_voc_cid_t viewId, 
                   avocadodb::velocypack::Slice const& data)
    : _databaseId(databaseId),
      _viewId(viewId),
      _data(data.begin()),
      _type(type) {

    TRI_ASSERT(databaseId > 0);
    TRI_ASSERT(viewId > 0);
  }

  ~MMFilesViewMarker() = default;

  /// @brief returns the marker type 
  MMFilesMarkerType type() const override final { return _type; }
  
  /// @brief returns the datafile id
  /// this is always 0 for this type of marker, as the marker is not
  /// yet in any datafile
  TRI_voc_fid_t fid() const override final { return 0; }
  
  /// @brief returns the marker size 
  uint32_t size() const override final { 
    return static_cast<uint32_t>(MMFilesDatafileHelper::VPackOffset(_type) + _data.byteSize());
  }

  /// @brief store the marker in the memory region starting at mem
  void store(char* mem) const override final { 
    // store database id
    encoding::storeNumber<decltype(_databaseId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::DatabaseIdOffset(_type), _databaseId, sizeof(decltype(_databaseId)));
    // store view id
    encoding::storeNumber<decltype(_viewId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::ViewIdOffset(_type), _viewId, sizeof(decltype(_viewId)));
    // store VPack
    memcpy(mem + MMFilesDatafileHelper::VPackOffset(_type), _data.begin(), static_cast<size_t>(_data.byteSize()));
  }

 private:
  TRI_voc_tick_t _databaseId;
  TRI_voc_cid_t _viewId;
  avocadodb::velocypack::Slice _data;
  MMFilesMarkerType _type;
};

/// @brief a marker used for transaction-related operations
class MMFilesTransactionMarker : public MMFilesWalMarker {
 public:
  MMFilesTransactionMarker(MMFilesMarkerType type, 
                    TRI_voc_tick_t databaseId, 
                    TRI_voc_tid_t transactionId)
    : _databaseId(databaseId),
      _transactionId(transactionId),
      _type(type) {
    TRI_ASSERT(databaseId > 0);
    TRI_ASSERT(transactionId > 0);
  }

  ~MMFilesTransactionMarker() = default;

  /// @brief returns the marker type 
  MMFilesMarkerType type() const override final { return _type; }
  
  /// @brief returns the datafile id
  /// this is always 0 for this type of marker, as the marker is not
  /// yet in any datafile
  TRI_voc_fid_t fid() const override final { return 0; }
  
  /// @brief returns the marker size 
  uint32_t size() const override final {
    // these markers do not have any VPack payload 
    return static_cast<uint32_t>(MMFilesDatafileHelper::VPackOffset(_type));
  }

  /// @brief store the marker in the memory region starting at mem
  void store(char* mem) const override final { 
    // store database id
    encoding::storeNumber<decltype(_databaseId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::DatabaseIdOffset(_type), _databaseId, sizeof(decltype(_databaseId)));
    // store transaction id
    encoding::storeNumber<decltype(_transactionId)>(reinterpret_cast<uint8_t*>(mem) + MMFilesDatafileHelper::TransactionIdOffset(_type), _transactionId, sizeof(decltype(_transactionId)));
  }

 private:
  TRI_voc_tick_t _databaseId;
  TRI_voc_tid_t _transactionId;
  MMFilesMarkerType _type;
};

}

#endif
