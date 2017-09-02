////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 AvocadoDB GmbH, Cologne, Germany
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
/// Copyright holder is AvocadoDB GmbH, Cologne, Germany
///
/// @author Max Neunhoeffer
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_REST_HANDLER_REST_SHUTDOWN_HANDLER_H
#define AVOCADOD_REST_HANDLER_REST_SHUTDOWN_HANDLER_H 1

#include "RestHandler/RestBaseHandler.h"

#include "ApplicationFeatures/ApplicationServer.h"
#include "Rest/HttpResponse.h"

namespace avocadodb {
class RestShutdownHandler : public RestBaseHandler {
 public:
  explicit RestShutdownHandler(GeneralRequest*, GeneralResponse*);

 public:
  char const* name() const override final { return "RestShutdownHandler"; }
  bool isDirect() const override;
  RestStatus execute() override;
};
}

#endif