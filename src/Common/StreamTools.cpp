// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "StreamTools.h"
#include <stdexcept>
#include "IInputStream.h"
#include "IOutputStream.h"

#include <Xi/ExternalIncludePush.h>
#include <boost/endian/conversion.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Encoding/VarInt.hh>

namespace Common {

void read(IInputStream& in, void* data, size_t size) {
  while (size > 0) {
    size_t readSize = in.readSome(data, size);
    if (readSize == 0) {
      throw std::runtime_error("Failed to read from IInputStream");
    }

    data = static_cast<uint8_t*>(data) + readSize;
    size -= readSize;
  }
}

namespace {
template <typename _IntegerT>
void genericReadVarint(Common::IInputStream& in, _IntegerT& value) {
  using namespace Xi;
  std::array<Byte, Encoding::VarInt::maximumEncodingSize<_IntegerT>()> buffer;
  size_t i = 0;
  Common::read(in, buffer[i++]);
  while (Encoding::VarInt::hasSuccessor(buffer[i - 1])) {
    const size_t index = i++;
    if (!(index < buffer.size())) {
      throw std::runtime_error{"varint buffer overflow."};
    }
    Common::read(in, buffer[index]);
  }
  Encoding::VarInt::decode(asByteSpan(buffer.data(), i), value).throwOnError();
}

template <typename _IntegerT>
void genericWriteVarint(Common::IOutputStream& out, _IntegerT value) {
  using namespace Xi;
  std::array<Byte, Encoding::VarInt::maximumEncodingSize<_IntegerT>()> buffer;
  const auto written = Encoding::VarInt::encode(value, buffer).valueOrThrow();
  Common::write(out, buffer.data(), written);
}
}  // namespace

void read(IInputStream& in, int8_t& value) {
  read(in, &value, sizeof(value));
  boost::endian::little_to_native_inplace(value);
}

void read(IInputStream& in, int16_t& value) {
  read(in, &value, sizeof(value));
  boost::endian::little_to_native_inplace(value);
}

void read(IInputStream& in, int32_t& value) {
  read(in, &value, sizeof(value));
  boost::endian::little_to_native_inplace(value);
}

void read(IInputStream& in, int64_t& value) {
  read(in, &value, sizeof(value));
  boost::endian::little_to_native_inplace(value);
}

void read(IInputStream& in, uint8_t& value) {
  read(in, &value, sizeof(value));
}

void read(IInputStream& in, uint16_t& value) {
  read(in, &value, sizeof(value));
  boost::endian::little_to_native_inplace(value);
}

void read(IInputStream& in, uint32_t& value) {
  read(in, &value, sizeof(value));
  boost::endian::little_to_native_inplace(value);
}

void read(IInputStream& in, uint64_t& value) {
  read(in, &value, sizeof(value));
  boost::endian::little_to_native_inplace(value);
}

void read(IInputStream& in, std::vector<uint8_t>& data, size_t size) {
  data.resize(size);
  read(in, data.data(), size);
}

void read(IInputStream& in, std::string& data, size_t size) {
  std::vector<char> temp(size);
  read(in, temp.data(), size);
  data.assign(temp.data(), size);
}

void write(IOutputStream& out, const void* data, size_t size) {
  while (size > 0) {
    size_t writtenSize = out.writeSome(data, size);
    if (writtenSize == 0) {
      throw std::runtime_error("Failed to write to IOutputStream");
    }

    data = static_cast<const uint8_t*>(data) + writtenSize;
    size -= writtenSize;
  }
}

void write(IOutputStream& out, int8_t value) {
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, int16_t value) {
  boost::endian::native_to_little_inplace(value);
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, int32_t value) {
  boost::endian::native_to_little_inplace(value);
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, int64_t value) {
  boost::endian::native_to_little_inplace(value);
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint8_t value) {
  boost::endian::native_to_little_inplace(value);
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint16_t value) {
  boost::endian::native_to_little_inplace(value);
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint32_t value) {
  boost::endian::native_to_little_inplace(value);
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, uint64_t value) {
  boost::endian::native_to_little_inplace(value);
  write(out, &value, sizeof(value));
}

void write(IOutputStream& out, const std::vector<uint8_t>& data) {
  write(out, data.data(), data.size());
}

void write(IOutputStream& out, const std::string& data) {
  write(out, data.data(), data.size());
}

void readVarint(IInputStream& in, uint8_t& value) {
  genericReadVarint(in, value);
}
void readVarint(IInputStream& in, uint16_t& value) {
  genericReadVarint(in, value);
}
void readVarint(IInputStream& in, uint32_t& value) {
  genericReadVarint(in, value);
}
void readVarint(IInputStream& in, uint64_t& value) {
  genericReadVarint(in, value);
}
void readVarint(IInputStream& in, int8_t& value) {
  genericReadVarint(in, value);
}
void readVarint(IInputStream& in, int16_t& value) {
  genericReadVarint(in, value);
}
void readVarint(IInputStream& in, int32_t& value) {
  genericReadVarint(in, value);
}
void readVarint(IInputStream& in, int64_t& value) {
  genericReadVarint(in, value);
}
void writeVarint(IOutputStream& out, uint8_t value) {
  genericWriteVarint(out, value);
}
void writeVarint(IOutputStream& out, uint16_t value) {
  genericWriteVarint(out, value);
}
void writeVarint(IOutputStream& out, uint32_t value) {
  genericWriteVarint(out, value);
}
void writeVarint(IOutputStream& out, uint64_t value) {
  genericWriteVarint(out, value);
}
void writeVarint(IOutputStream& out, int8_t value) {
  genericWriteVarint(out, value);
}
void writeVarint(IOutputStream& out, int16_t value) {
  genericWriteVarint(out, value);
}
void writeVarint(IOutputStream& out, int32_t value) {
  genericWriteVarint(out, value);
}
void writeVarint(IOutputStream& out, int64_t value) {
  genericWriteVarint(out, value);
}

}  // namespace Common
