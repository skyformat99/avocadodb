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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_REST_HANDLER_REST_PLEASE_UPGRADE_HANDLER_H
#define AVOCADOD_REST_HANDLER_REST_PLEASE_UPGRADE_HANDLER_H 1

#include "GeneralServer/RestHandler.h"

namespace avocadodb {
class RestPleaseUpgradeHandler : public rest::RestHandler {
 public:
  explicit RestPleaseUpgradeHandler(GeneralRequest*, GeneralResponse*);

 public:
  char const* name() const override final { return "RestPleaseUpgradeHandler"; }
  bool isDirect() const override;
  RestStatus execute() override;
  void handleError(const basics::Exception&) override;
};
}

#endif
