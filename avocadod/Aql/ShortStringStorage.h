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

#ifndef AVOCADOD_AQL_SHORT_STRING_STORAGE_H
#define AVOCADOD_AQL_SHORT_STRING_STORAGE_H 1

#include "Basics/Common.h"

namespace avocadodb {
namespace aql {

struct ResourceMonitor;

class ShortStringStorage {
 public:
  ShortStringStorage(ShortStringStorage const&) = delete;
  ShortStringStorage& operator=(ShortStringStorage const&) = delete;

  ShortStringStorage(ResourceMonitor*, size_t);

  ~ShortStringStorage();

  /// @brief register a short string
  char* registerString(char const*, size_t);

  /// @brief allocate a new block of memory
  void allocateBlock();

 public:
  /// @brief maximum length of strings in short string storage
  static size_t const MaxStringLength;

 private:
  ResourceMonitor* _resourceMonitor;

  /// @brief already allocated string blocks
  std::vector<char*> _blocks;

  /// @brief size of each block
  size_t const _blockSize;

  /// @brief offset into current block
  char* _current;

  /// @brief end of current block
  char* _end;
};
}
}

#endif
