////////////////////////////////////////////////////////////////////////////////
/// @brief Library to build up VPack documents.
///
/// DISCLAIMER
///
/// Copyright 2015 ArangoDB GmbH, Cologne, Germany
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
/// @author Max Neunhoeffer
/// @author Jan Steemann
/// @author Copyright 2015, ArangoDB GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef VELOCYPACK_BASICS_H
#define VELOCYPACK_BASICS_H 1

#include <cstring>
#include <new>

namespace avocadodb {
namespace velocypack {

// classes from Basics.h are for internal use only and are not exposed here

// prevent copying
class NonCopyable {
 public:
  NonCopyable() = default;
  ~NonCopyable() = default;
 private:
  NonCopyable (NonCopyable const&) = delete;
  NonCopyable& operator= (NonCopyable const&) = delete;
};


#ifdef _WIN32
// turn off warnings about unimplemented exception specifications
#pragma warning(push)
#pragma warning(disable : 4290)
#endif

// prevent heap allocation
struct NonHeapAllocatable {
  void* operator new(std::size_t) = delete; 
  void operator delete(void*) noexcept = delete; 
  void* operator new[](std::size_t) = delete; 
  void operator delete[](void*) noexcept = delete;  
};

#ifdef _WIN32
#pragma warning(pop)
#endif

}  // namespace avocadodb::velocypack
}  // namespace avocadodb

#endif
