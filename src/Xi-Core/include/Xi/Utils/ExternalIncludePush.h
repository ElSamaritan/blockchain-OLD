/*!
 * Include this header before you include external headers yielding warnings.
 *
 * Note: You must include the corresponding Pop header once you included all external headers.
 */

#include <leathers/push>
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"
#endif
#include <leathers/all>
#if defined(_MSC_VER)
#pragma warning(disable : 4267)
#pragma warning(disable : 4456)
#pragma warning(disable : 4458)
#pragma warning(disable : 4067)
#pragma warning(disable : 4706)
#define NOGDI
#endif
