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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGODB_BASICS_THREADS_H
#define ARANGODB_BASICS_THREADS_H 1

#include "Basics/Common.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief posix threads
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_HAVE_POSIX_THREADS
#include "Basics/threads-posix.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief windows threads
////////////////////////////////////////////////////////////////////////////////

#ifdef TRI_HAVE_WIN32_THREADS
#include "Basics/threads-win32.h"
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief initializes a thread
////////////////////////////////////////////////////////////////////////////////

void TRI_InitThread(TRI_thread_t*);

////////////////////////////////////////////////////////////////////////////////
/// @brief starts a thread
////////////////////////////////////////////////////////////////////////////////

bool TRI_StartThread(TRI_thread_t*, TRI_tid_t*, char const*,
                     void (*starter)(void*), void* data);

////////////////////////////////////////////////////////////////////////////////
/// @brief checks if we are the thread
////////////////////////////////////////////////////////////////////////////////

bool TRI_IsSelfThread(TRI_thread_t* thread);

////////////////////////////////////////////////////////////////////////////////
/// @brief sets the process affinity
////////////////////////////////////////////////////////////////////////////////

void TRI_SetProcessorAffinity(TRI_thread_t*, size_t core);

// SHOULD BE REMOVED
void TRI_InitThread(TRI_thread_t* thread);
int TRI_JoinThread(TRI_thread_t* thread);

#endif
