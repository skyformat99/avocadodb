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
/// @author Kaveh Vahedipour
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_CONSENSUS_FAILED_FOLLOWER_H
#define AVOCADOD_CONSENSUS_FAILED_FOLLOWER_H 1

#include "Job.h"
#include "Supervision.h"

namespace avocadodb {
namespace consensus {

struct FailedFollower : public Job {
  FailedFollower(Node const& snapshot, AgentInterface* agent,
                 std::string const& jobId,
                 std::string const& creator = std::string(),
                 std::string const& database = std::string(),
                 std::string const& collection = std::string(),
                 std::string const& shard = std::string(),
                 std::string const& from = std::string());

  FailedFollower(Node const& snapshot, AgentInterface* agent,
                 JOB_STATUS status, std::string const& jobId);

  virtual ~FailedFollower();

  virtual bool create(std::shared_ptr<VPackBuilder> b = nullptr) override final;
  virtual void run() override final;
  virtual bool start() override final;
  virtual JOB_STATUS status() override final;
  virtual Result abort() override final;

  std::string _database;
  std::string _collection;
  std::string _shard;
  std::string _from;
  std::string _to;
  std::chrono::system_clock::time_point _created;
};
}
}  // namespaces

#endif