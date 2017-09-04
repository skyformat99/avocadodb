////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2017 ArangoDB GmbH, Cologne, Germany
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
/// @author Simon Grätzer
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_ROCKSDB_ROCKSDB_AQL_FUNCTIONS_H
#define AVOCADOD_ROCKSDB_ROCKSDB_AQL_FUNCTIONS_H 1

#include "Aql/Functions.h"
#include "Basics/Common.h"

namespace avocadodb {
namespace aql {
struct Function;
}

struct RocksDBAqlFunctions : public aql::Functions {
  static aql::AqlValue Fulltext(avocadodb::aql::Query*, transaction::Methods*,
                                aql::VPackFunctionParameters const&);

  static aql::AqlValue Near(avocadodb::aql::Query*, transaction::Methods*,
                            aql::VPackFunctionParameters const&);

  static aql::AqlValue Within(avocadodb::aql::Query*, transaction::Methods*,
                              aql::VPackFunctionParameters const&);

  static void registerResources();
};

}  // namespace avocadodb

#endif
