'use strict';

// //////////////////////////////////////////////////////////////////////////////
// / @brief initialize a new database
// /
// / @file
// /
// / DISCLAIMER
// /
// / Copyright 2014 AvocadoDB GmbH, Cologne, Germany
// /
// / Licensed under the Apache License, Version 2.0 (the "License")
// / you may not use this file except in compliance with the License.
// / You may obtain a copy of the License at
// /
// /     http://www.apache.org/licenses/LICENSE-2.0
// /
// / Unless required by applicable law or agreed to in writing, software
// / distributed under the License is distributed on an "AS IS" BASIS,
// / WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// / See the License for the specific language governing permissions and
// / limitations under the License.
// /
// / Copyright holder is AvocadoDB GmbH, Cologne, Germany
// /
// / @author Dr. Frank Celler
// / @author Copyright 2014, AvocadoDB GmbH, Cologne, Germany
// //////////////////////////////////////////////////////////////////////////////

// //////////////////////////////////////////////////////////////////////////////
// / @brief initialize a new database
// //////////////////////////////////////////////////////////////////////////////

(function () {
  const internal = require('internal');

  // do not overwrite arguments set from the calling C++ code
  if (!global.UPGRADE_ARGS) {
    global.UPGRADE_ARGS = {};
  }

  // merge in our arguments
  global.UPGRADE_ARGS.isCluster = true;
  global.UPGRADE_ARGS.isCoordinator = true;
  global.UPGRADE_ARGS.isRelaunch = false;

  // run the local upgrade-database script
  const res = internal.loadStartup('server/bootstrap/local-database.js');

  return res;
}());
