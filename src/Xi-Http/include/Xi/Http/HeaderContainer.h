#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <initializer_list>

#include <boost/optional.hpp>

#include <Xi/Global.h>

#include "Xi/Http/AuthenticationType.h"
#include "Xi/Http/ContentType.h"
#include "Xi/Http/ContentEncoding.h"
#include "Xi/Http/BasicCredentials.h"

namespace Xi {
namespace Http {
class HeaderContainer final {
 public:
  /*!
   * HTTP headers allow the client and the server to pass additional information with the request or the response. An
   * HTTP header consists of its case-insensitive name followed by a colon ':', then by its value (without line breaks).
   * Leading white space before the value is ignored.
   *
   * Documentation is taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers
   */
  enum Header {
    WWWAuthenticate = 329,  ///< The HTTP WWW-Authenticate response header defines the authentication method that should
                            ///< be used to gain access to a resource.
    Authorization = 39,     ///< Contains the credentials to authenticate a user agent with a server.
    Connection = 59,     ///< Controls whether the network connection stays open after the current transaction finishes.
    KeepAlive = 160,     ///< Controls how long a persistent connection should stay open.
    Accept = 2,          ///< Informs the server about the types of data that can be sent back. It is MIME-type.
    AcceptCharset = 4,   ///< Informs the server about which character set the client is able to understand.
    AcceptEncoding = 6,  ///< Informs the server about the encoding algorithm, usually a compression algorithm, that can
                         ///< be used on the resource sent back.
    ContentType = 78,    ///< Indicates the media type of the resource.
    ContentEncoding = 65,  ///< The Content-Encoding entity header is used to compress the media-type. When present, its
                           ///< value indicates which encodings were applied to the entity-body. It lets the client know
                           ///< how to decode in order to obtain the media-type referenced by the Content-Type header.
    Location = 176,        ///< Indicates the URL to redirect a page to.
    Allow = 22,            ///< Lists the set of HTTP request methods support by a resource.
    Server = 286           ///< Contains information about the software used by the origin server to handle the request.
  };

  using iterator = std::map<Header, std::string>::iterator;
  using const_iterator = std::map<Header, std::string>::const_iterator;

 public:
  XI_DEFAULT_COPY(HeaderContainer);
  XI_DEFAULT_MOVE(HeaderContainer);

  HeaderContainer() = default;
  ~HeaderContainer() = default;

  /*!
   * \brief setRequiredAuthenticationScheme indicates that a request schema is required in order to send requests
   * \param authType the authentication type expected by the server
   */
  void setRequiredAuthenticationScheme(Xi::Http::AuthenticationType authType);

  boost::optional<AuthenticationType> requiredAuthenticationScheme() const;

  /*!
   * \brief setBasicAuthorization sets the authorization credentials required to authenticate a request with the server
   * \exception std::invalid_argument the provided username is not valid (empty or contains ':')
   */
  void setBasicAuthorization(const std::string& username, const std::string& password = "");

  /*!
   * \brief setBasicAuthorization sets the authorization credentials required to authenticate a request with the server
   */
  void setBasicAuthorization(const BasicCredentials& credentials);

  /*!
   * \brief basicAuthorization returns the basic credentials, if set, if the value is set but invalid an exception is
   * thrown
   */
  boost::optional<BasicCredentials> basicAuthorization() const;

  void setContentType(::Xi::Http::ContentType contentType);

  /*!
   * \brief setAcceptedContentEncoding sets the accepted encodings from the client (ie. gzip, identity)
   * \throws std::invalid_argument no encoding provided (size == 0)
   */
  void setAcceptedContentEncodings(std::initializer_list<::Xi::Http::ContentEncoding> encodings);

  /*!
   * \brief acceptedContentEncodings returns the compressions supported by the client.
   */
  boost::optional<std::vector<::Xi::Http::ContentEncoding>> acceptedContentEncodings() const;

  /*!
   * \brief setContentEncoding indicates which encoding was used for a response.
   */
  void setContentEncoding(::Xi::Http::ContentEncoding encoding);

  /*!
   * \brief contentEncoding returns the compression used for the response, if set
   */
  boost::optional<::Xi::Http::ContentEncoding> contentEncoding() const;

  /*!
   * \brief contentType returns the content type if set, if set but invalid/unknown an exception is thrown
   */
  boost::optional<::Xi::Http::ContentType> contentType() const;

  boost::optional<std::string> location() const;

  const_iterator begin() const;
  const_iterator end() const;

  boost::optional<std::string> get(Header header) const;

  void set(Header header, const std::string& raw);

  bool contains(Header header) const;

 private:
  std::map<Header, std::string> m_rawHeaders;
};
}  // namespace Http
}  // namespace Xi
