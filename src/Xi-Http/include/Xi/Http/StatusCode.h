/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#pragma once

#include <string>

#include <Xi/Utils/String.h>

namespace Xi {
namespace Http {
/*!
 * \brief HTTP response status codes indicate whether a specific HTTP request has been successfully completed. Responses
 * are grouped in five classes: informational responses, successful responses, redirects, client errors, and servers
 * errors. Status codes are defined by section 10 of RFC 2616.
 *
 * Documentation is taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
 */
enum struct StatusCode {
  Ok = 200,       ///< The request has succeeded. The meaning of a success varies depending on the HTTP method:
                  ///< GET: The resource has been fetched and is transmitted in the message body.
                  ///< HEAD: The entity headers are in the message body.
                  ///< PUT or POST: The resource describing the result of the action is transmitted in the message body.
                  ///< TRACE: The message body contains the request message as received by the server
  Created = 201,  ///< The request has succeeded and a new resource has been created as a result of it. This is
                  ///< typically the response sent after a POST request, or after some PUT requests.
  Accepted =
      202,  ///< The request has been received but not yet acted upon. It is non-committal, meaning that there is no way
            ///< in HTTP to later send an asynchronous response indicating the outcome of processing the request. It is
            ///< intended for cases where another process or server handles the request, or for batch processing.
  NoContent = 204,  ///< There is no content to send for this request, but the headers may be useful. The user-agent may
                    ///< update its cached headers for this resource with the new ones.

  MovedPermanently = 301,  ///< This response code means that the URI of the requested resource has been changed.
                           ///< Probably, the new URI would be given in the response.
  Found = 302,     ///< This response code means that the URI of requested resource has been changed temporarily. New
                   ///< changes in the URI might be made in the future. Therefore, this same URI should be used by the
                   ///< client in future requests.
  SeeOther = 303,  ///< The server sent this response to direct the client to get the requested resource at another URI
                   ///< with a GET request.
  TemporaryRedirect =
      307,  ///< The server sends this response to direct the client to get the requested resource at another URI with
            ///< same method that was used in the prior request. This has the same semantics as the 302 Found HTTP
            ///< response code, with the exception that the user agent must not change the HTTP method used: If a POST
            ///< was used in the first request, a POST must be used in the second request.
  PermanentRedirect =
      308,  ///< This means that the resource is now permanently located at another URI, specified by the Location: HTTP
            ///< Response header. This has the same semantics as the 301 Moved Permanently HTTP response code, with the
            ///< exception that the user agent must not change the HTTP method used: If a POST was used in the first
            ///< request, a POST must be used in the second request.

  BadRequest = 400,  ///< This response means that server could not understand the request due to invalid syntax.
  Unauthorized =
      401,          ///< Although the HTTP standard specifies "unauthorized", semantically this response means
                    ///< "unauthenticated". That is, the client must authenticate itself to get the requested response.
  Forbidden = 403,  ///< The client does not have access rights to the content, i.e. they are unauthorized, so server is
                    ///< rejecting to give proper response. Unlike 401, the client's identity is known to the server.
  NotFound =
      404,  ///< The server can not find requested resource. In the browser, this means the URL is not recognized. In an
            ///< API, this can also mean that the endpoint is valid but the resource itself does not exist. Servers may
            ///< also send this response instead of 403 to hide the existence of a resource from an unauthorized client.
            ///< This response code is probably the most famous one due to its frequent occurence on the web.
  MethodNotAllowed = 405,  ///< The request method is known by the server but has been disabled and cannot be used. For
                           ///< example, an API may forbid DELETE-ing a resource. The two mandatory methods, GET and
                           ///< HEAD, must never be disabled and should not return this error code.
  RequestTimeout = 408,    ///< This response is sent on an idle connection by some servers, even without any previous
                           ///< request by the client. It means that the server would like to shut down this unused
  ///< connection. This response is used much more since some browsers, like Chrome, Firefox 27+,
  ///< or IE9, use HTTP pre-connection mechanisms to speed up surfing. Also note that some
  ///< servers merely shut down the connection without sending this message.
  Gone = 410,  ///< This response would be sent when the requested content has been permanently deleted from server,
               ///< with no forwarding address. Clients are expected to remove their caches and links to the resource.
               ///< The HTTP specification intends this status code to be used for "limited-time, promotional services".
               ///< APIs should not feel compelled to indicate resources that have been deleted with this status code.
  LengthRequired = 411,  ///< Server rejected the request because the Content-Length header field is not defined and the
                         ///< server requires it.
  UnsupportedMediaType = 415,  ///< The media format of the requested data is not supported by the server, so the server
                               ///< is rejecting the request.
  TooManyRequests = 429,       ///< The user has sent too many requests in a given amount of time ("rate limiting").

  InternalServerError = 500,  ///< The server has encountered a situation it doesn't know how to handle.
  NotImplemented =
      501,  ///< The request method is not supported by the server and cannot be handled. The only methods that servers
            ///< are required to support (and therefore that must not return this code) are GET and HEAD.
  BadGateway = 502,  ///< This error response means that the server, while working as a gateway to get a response needed
                     ///< to handle the request, got an invalid response.
  ServiceUnavailable =
      503,  ///< The server is not ready to handle the request. Common causes are a server that is down for maintenance
            ///< or that is overloaded. Note that together with this response, a user-friendly page explaining the
            ///< problem should be sent. This responses should be used for temporary conditions and the Retry-After:
            ///< HTTP header should, if possible, contain the estimated time before the recovery of the service. The
            ///< webmaster must also take care about the caching-related headers that are sent along with this response,
            ///< as these temporary condition responses should usually not be cached.
  GatewayTimeout =
      504,  ///< This error response is given when the server is acting as a gateway and cannot get a response in time.
  HttpVersionNotSupported = 505,  ///< The HTTP version used in the request is not supported by the server.
  NetworkAuthenticationRequired =
      511,  ///< The 511 status code indicates that the client needs to authenticate to gain network access.
  NetworkConnectTimeout = 599  ///< The connection timed out.
};
}  // namespace Http
std::string to_string(Http::StatusCode status);
}  // namespace Xi
