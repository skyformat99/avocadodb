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

#ifndef AVOCADODB_REST_GENERAL_RESPONSE_H
#define AVOCADODB_REST_GENERAL_RESPONSE_H 1

#include "Basics/Common.h"

#include "Basics/StaticStrings.h"
#include "Basics/StringBuffer.h"
#include "Basics/StringUtils.h"
#include "GeneralRequest.h"
#include "Endpoint/Endpoint.h"
#include "Logger/Logger.h"
#include "Rest/CommonDefines.h"

namespace avocadodb {
namespace velocypack {
struct Options;
class Slice;
}

using rest::ContentType;
using rest::ConnectionType;
using rest::ResponseCode;

class GeneralRequest;

class GeneralResponse {
  GeneralResponse() = delete;
  GeneralResponse(GeneralResponse const&) = delete;
  GeneralResponse& operator=(GeneralResponse const&) = delete;

 public:
  // converts the response code to a string for delivering to a http client.
  static std::string responseString(ResponseCode);

  // converts the response code string to the internal code
  static ResponseCode responseCode(std::string const& str);

  // response code from integer error code
  static ResponseCode responseCode(int);

  /// @brief set content-type this sets the contnt type like you expect it
  void setContentType(ContentType type) { _contentType = type; }

  /// @brief set content-type from a string. this should only be used in
  /// cases when the content-type is user-defined
  /// this is a functionality so that user can set a type like application/zip
  /// from java script code the ContentType will be CUSTOM!!
  void setContentType(std::string const& contentType) {
    _headers[avocadodb::StaticStrings::ContentTypeHeader] = contentType;
    _contentType = ContentType::CUSTOM;
  }

  void setContentType(std::string&& contentType) {
    _headers[avocadodb::StaticStrings::ContentTypeHeader] =
        std::move(contentType);
    _contentType = ContentType::CUSTOM;
  }

  void setConnectionType(ConnectionType type) { _connectionType = type; }
  void setContentTypeRequested(ContentType type) {
    _contentTypeRequested = type;
  }

  virtual avocadodb::Endpoint::TransportType transportType() = 0;

 protected:
  explicit GeneralResponse(ResponseCode);

 public:
  virtual ~GeneralResponse() {}

 public:
  // response codes are http response codes, but they are used in other
  // protocols as well
  ResponseCode responseCode() const { return _responseCode; }
  void setResponseCode(ResponseCode responseCode) {
    _responseCode = responseCode;
  }

  void setHeaders(std::unordered_map<std::string, std::string>&& headers) {
    _headers = std::move(headers);
  }

  std::unordered_map<std::string, std::string> headers() const {
    return _headers;
  }
  
  // adds a header. the header field name will be lower-cased
  void setHeader(std::string const& key, std::string const& value) {
    _headers[basics::StringUtils::tolower(key)] = value;
  }

  // adds a header. the header field name must be lower-cased
  void setHeaderNC(std::string const& key, std::string const& value) {
    _headers[key] = value;
  }

  // adds a header. the header field name must be lower-cased
  void setHeaderNC(std::string const& key, std::string&& value) {
    _headers[key] = std::move(value);
  }
  
  // adds a header if not set. the header field name must be lower-cased
  void setHeaderNCIfNotSet(std::string const& key, std::string const& value) {
    if (_headers.find(key) != _headers.end()) {
      // already set
      return;
    }
    _headers.emplace(key, value);
  }
  
 public:
  virtual uint64_t messageId() const { return 1; }

  virtual void reset(ResponseCode) = 0;

  // generates the response body, sets the content type; this might
  // throw an error
  void setPayload(VPackSlice slice, bool generateBody,
                  VPackOptions const& options = VPackOptions::Options::Defaults,
                  bool resolveExternals = true) {
    _generateBody = generateBody;
    addPayload(slice, &options, resolveExternals);
  }
  // Payload needs to be of type: VPackSlice const&
  // or VPackBuffer<uint8_t>&&
  template <typename Payload>
  void setPayload(Payload&& payload, bool generateBody,
                  VPackOptions const& options = VPackOptions::Options::Defaults,
                  bool resolveExternals = true) {
    _generateBody = generateBody;
    addPayload(std::forward<Payload>(payload), &options, resolveExternals);
  }

  void addPayloadPreconditions() { 
    if (_vpackPayloads.size() != 0) {
      LOG_TOPIC(ERR, avocadodb::Logger::FIXME) << "Payload set twice";
      TRI_ASSERT(_vpackPayloads.size() == 0);
    }
  }
  virtual void addPayloadPreHook(bool inputIsBuffer, bool& resolveExternals,
                                 bool& skipBody) {}
  void addPayload(VPackSlice const&,
                  avocadodb::velocypack::Options const* = nullptr,
                  bool resolve_externals = true);
  void addPayload(VPackBuffer<uint8_t>&&,
                  avocadodb::velocypack::Options const* = nullptr,
                  bool resolve_externals = true);
  virtual void addPayloadPostHook(VPackSlice const&,
                                  avocadodb::velocypack::Options const*,
                                  bool resolveExternals, bool bodySkipped) {}

  virtual int reservePayload(std::size_t size) { return TRI_ERROR_NO_ERROR; }
  bool generateBody() const { return _generateBody; };  // used for head
  virtual bool setGenerateBody(bool) {
    return _generateBody;
  };  // used for head
      // resonses
  void setOptions(VPackOptions options) { _options = std::move(options); };

 protected:
  ResponseCode _responseCode;  // http response code
  std::unordered_map<std::string, std::string>
      _headers;  // headers/metadata map

  std::vector<VPackBuffer<uint8_t>> _vpackPayloads;
  std::size_t _numPayloads;
  ContentType _contentType;
  ConnectionType _connectionType;
  velocypack::Options _options;
  bool _generateBody;
  ContentType _contentTypeRequested;
};
}

#endif
