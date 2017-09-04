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
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADODB_GRAPH_ATTRIBUTE_WEIGHT_SHORTEST_PATH_FINDER_H
#define AVOCADODB_GRAPH_ATTRIBUTE_WEIGHT_SHORTEST_PATH_FINDER_H 1

#include "Basics/Mutex.h"
#include "Basics/MutexLocker.h"
#include "Basics/StringRef.h"

#include "Graph/ShortestPathFinder.h"
#include "Graph/ShortestPathPriorityQueue.h"

#include <thread>

namespace avocadodb {

class ManagedDocumentResult;

namespace graph {

struct EdgeDocumentToken;
struct ShortestPathOptions;

class AttributeWeightShortestPathFinder : public ShortestPathFinder {
 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief Step, one position with a predecessor and the edge
  //////////////////////////////////////////////////////////////////////////////

  struct Step {
   private:
    double _weight;

   public:
    avocadodb::StringRef _vertex;
    avocadodb::StringRef _predecessor;
    std::unique_ptr<avocadodb::graph::EdgeDocumentToken> _edge;
    bool _done;

    Step();

    Step(avocadodb::StringRef const& vert,
         avocadodb::StringRef const& pred, double weig,
         std::unique_ptr<EdgeDocumentToken>&& edge);

    double weight() const { return _weight; }

    void setWeight(double w) { _weight = w; }

    avocadodb::StringRef const& getKey() const { return _vertex; }
  };

  //////////////////////////////////////////////////////////////////////////////
  /// @brief edge direction
  //////////////////////////////////////////////////////////////////////////////

  typedef enum { FORWARD, BACKWARD } Direction;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief our specialization of the priority queue
  //////////////////////////////////////////////////////////////////////////////

  typedef avocadodb::graph::ShortestPathPriorityQueue<
      avocadodb::StringRef, Step, double>
      PQueue;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief information for each thread
  //////////////////////////////////////////////////////////////////////////////

  struct ThreadInfo {
    PQueue _pq;
    avocadodb::Mutex _mutex;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// @brief a Dijkstra searcher for the multi-threaded search
  //////////////////////////////////////////////////////////////////////////////

  /*
  class SearcherTwoThreads {
    AttributeWeightShortestPathFinder* _pathFinder;
    ThreadInfo& _myInfo;
    ThreadInfo& _peerInfo;
    avocadodb::velocypack::Slice _start;
    ExpanderFunction _expander;
    std::string _id;

   public:
    SearcherTwoThreads(AttributeWeightShortestPathFinder* pathFinder,
                       ThreadInfo& myInfo, ThreadInfo& peerInfo,
                       avocadodb::velocypack::Slice const& start,
                       ExpanderFunction expander, std::string const& id);

   private:
    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Insert a neighbor to the todo list.
    ////////////////////////////////////////////////////////////////////////////////
    void insertNeighbor(Step* step, double newWeight);

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Lookup our current vertex in the data of our peer.
    ////////////////////////////////////////////////////////////////////////////////

    void lookupPeer(avocadodb::velocypack::Slice& vertex, double weight);

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Search graph starting at Start following edges of the given
    /// direction only
    ////////////////////////////////////////////////////////////////////////////////

    void run();

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief start and join functions
    ////////////////////////////////////////////////////////////////////////////////

   public:
    void start();

    void join();

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief The thread object.
    ////////////////////////////////////////////////////////////////////////////////

   private:
    std::thread _thread;
  };
  */

  //////////////////////////////////////////////////////////////////////////////
  /// @brief a Dijkstra searcher for the single-threaded search
  //////////////////////////////////////////////////////////////////////////////

  class Searcher {
   public:
    Searcher(AttributeWeightShortestPathFinder* pathFinder, ThreadInfo& myInfo,
             ThreadInfo& peerInfo, avocadodb::StringRef const& start,
             bool isBackward);

   public:
    //////////////////////////////////////////////////////////////////////////////
    /// @brief Do one step only.
    //////////////////////////////////////////////////////////////////////////////

    bool oneStep();

   private:
    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Insert a neighbor to the todo list.
    ////////////////////////////////////////////////////////////////////////////////

    void insertNeighbor(Step* step, double newWeight);

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Lookup our current vertex in the data of our peer.
    ////////////////////////////////////////////////////////////////////////////////

    void lookupPeer(avocadodb::StringRef& vertex, double weight);

   private:
    AttributeWeightShortestPathFinder* _pathFinder;
    ThreadInfo& _myInfo;
    ThreadInfo& _peerInfo;
    avocadodb::StringRef _start;
    bool _isBackward;

  };

  // -----------------------------------------------------------------------------

  AttributeWeightShortestPathFinder(AttributeWeightShortestPathFinder const&) =
      delete;

  AttributeWeightShortestPathFinder& operator=(
      AttributeWeightShortestPathFinder const&) = delete;
  AttributeWeightShortestPathFinder() = delete;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief create the PathFinder
  //////////////////////////////////////////////////////////////////////////////

  explicit AttributeWeightShortestPathFinder(ShortestPathOptions* options);

  ~AttributeWeightShortestPathFinder();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Find the shortest path between start and target.
  ///        Only edges having the given direction are followed.
  ///        nullptr indicates there is no path.
  //////////////////////////////////////////////////////////////////////////////

  // Caller has to free the result
  // If this returns true there is a path, if this returns false there is no
  // path
  bool shortestPath(avocadodb::velocypack::Slice const& start,
                    avocadodb::velocypack::Slice const& target,
                    avocadodb::graph::ShortestPathResult& result,
                    std::function<void()> const& callback) override;

  void inserter(std::unordered_map<avocadodb::StringRef, size_t>& candidates,
                std::vector<Step*>& result, avocadodb::StringRef const& s,
                avocadodb::StringRef const& t, double currentWeight,
                std::unique_ptr<EdgeDocumentToken>&& edge);

  void expandVertex(bool isBackward, avocadodb::StringRef const& source,
                    std::vector<Step*>& result);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief return the shortest path between the start and target vertex,
  /// multi-threaded version using SearcherTwoThreads.
  //////////////////////////////////////////////////////////////////////////////

  // Caller has to free the result
  // If this returns true there is a path, if this returns false there is no
  // path

  /* Unused for now maybe reactived
  bool shortestPathTwoThreads(avocadodb::velocypack::Slice& start,
                              avocadodb::velocypack::Slice& target,
                              avocadodb::graph::ShortestPathResult& result);
  */

  //////////////////////////////////////////////////////////////////////////////
  /// @brief lowest total weight for a complete path found
  //////////////////////////////////////////////////////////////////////////////

  bool _highscoreSet;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief lowest total weight for a complete path found
  //////////////////////////////////////////////////////////////////////////////

  double _highscore;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief _bingo, flag that indicates termination
  //////////////////////////////////////////////////////////////////////////////

  std::atomic<bool> _bingo;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief result code. this is used to transport errors from sub-threads to
  /// the caller thread
  //////////////////////////////////////////////////////////////////////////////

  std::atomic<int> _resultCode;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief _resultMutex, this is used to protect access to the result data
  //////////////////////////////////////////////////////////////////////////////

  avocadodb::Mutex _resultMutex;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief _intermediate, one vertex on the shortest path found, flag
  /// indicates
  /// whether or not it was set.
  //////////////////////////////////////////////////////////////////////////////

  bool _intermediateSet;
  avocadodb::StringRef _intermediate;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief Reusable ManagedDocumentResult that temporarily takes
  ///        responsibility for one document.
  //////////////////////////////////////////////////////////////////////////////

 private:

  std::unique_ptr<ManagedDocumentResult> _mmdr;

  ShortestPathOptions* _options;
};

}  // namespace graph
}  // namespace avocadodb

#endif
