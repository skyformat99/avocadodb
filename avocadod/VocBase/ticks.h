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
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_VOC_BASE_TICKS_H
#define AVOCADOD_VOC_BASE_TICKS_H 1

#include "Basics/Common.h"
#include "VocBase/voc-types.h"

/// @brief create a new tick, using a hybrid logical clock
TRI_voc_tick_t TRI_HybridLogicalClock();

/// @brief create a new tick, using a hybrid logical clock, this variant
/// is supposed to be called when a time stamp is received in network
/// communications.
TRI_voc_tick_t TRI_HybridLogicalClock(TRI_voc_tick_t received);

/// @brief create a new tick
TRI_voc_tick_t TRI_NewTickServer();

/// @brief updates the tick counter, with lock
void TRI_UpdateTickServer(TRI_voc_tick_t);

/// @brief returns the current tick counter
TRI_voc_tick_t TRI_CurrentTickServer();

#endif