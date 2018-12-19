#pragma once

#include <string>

namespace Xi {
namespace Base64 {
/*!
 * \brief encode encodes a raw data block into a Bas64 encoded string
 * \param raw the binary data to encode
 * \return the Base64 encoding of the binary data
 */
std::string encode(const std::string& raw);

/*!
 * \brief decodes a Base64 encoded data string into its binary format
 * \param raw the Base64 encoded data block
 * \return the raw data blob thats was encoded by the given Base64 string
 */
std::string decode(const std::string& raw);
}  // namespace Base64
}  // namespace Xi
