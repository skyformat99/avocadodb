////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 ArangoDB GmbH, Cologne, Germany
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

#ifndef AVOCADOD_REST_HANDLER_PREGEL_HANDLER_H
#define AVOCADOD_REST_HANDLER_PREGEL_HANDLER_H 1

#include "RestHandler/RestVocbaseBaseHandler.h"

namespace avocadodb {
  class RestPregelHandler : public avocadodb::RestVocbaseBaseHandler {
  public:
    explicit RestPregelHandler(GeneralRequest*, GeneralResponse*);
    
  public:
    bool isDirect() const override { return false; }
    RestStatus execute() override;
    char const* name() const override {return "Pregel Rest Handler";}
  };
}

#endif
