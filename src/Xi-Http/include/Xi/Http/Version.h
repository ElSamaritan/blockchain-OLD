#pragma once

namespace Xi {
namespace Http {
enum struct Version {
  Http_0_9 =
      9,  ///< HTTP/0.9 – The one-line protocol \link
          ///< https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#HTTP0.9_%E2%80%93_The_one-line_protocol
  Http1_0 =
      10,  ///< HTTP/1.0 – Building extensibility \link
           ///< https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#HTTP1.0_%E2%80%93_Building_extensibility
  Http1_1 =
      11  ///< HTTP/1.1 – The standardized protocol \link
          ///< https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#HTTP1.1_%E2%80%93_The_standardized_protocol
};
}  // namespace Http
}  // namespace Xi
