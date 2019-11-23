/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
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

#include "Xi/Encoding/VarInt.hh"

#include <limits.h>

#include "Xi/Global.hh"

static const xi_byte_t xi_leb_next_mask = 0x80;
static const xi_byte_t xi_leb_next_shift = 7;
static const xi_byte_t xi_leb_negative_mask = 0x40;
static const xi_byte_t xi_leb_negative_shift = 6;
static const xi_byte_t xi_leb_first_byte_mask = 0xC0;

size_t xi_encoding_varint_decode_int8(const xi_byte_t *source, const size_t count, int8_t *out) {
  if (count < 1) {
    return XI_VARINT_DECODE_OUT_OF_MEMORY;
  }

  xi_byte_t byte = source[0];

  if (byte == xi_leb_negative_mask) {
    *out = CHAR_MIN;
    return 1;
  }

  *out = (int8_t)((byte & (~xi_leb_first_byte_mask)));
  size_t i = 1;

  if ((byte & xi_leb_next_mask) != 0) {
    for (size_t shift = xi_leb_negative_shift; 1; shift += xi_leb_next_shift) {
      if (i >= count) {
        return XI_VARINT_DECODE_OUT_OF_MEMORY;
      } else if (shift > 7) {
        return XI_VARINT_DECODE_OVERFLOW;
      }
      byte = source[i++];
      if (byte == 0) {
        if (i == 2 && source[0] == xi_leb_next_mask) {
          *out = CHAR_MAX;
          return 2;
        } else {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      const size_t bitsLeft = 7 - shift;
      if (bitsLeft < 8) {
        if (byte & (0xFF << bitsLeft)) {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      *out |= ((int8_t)((byte & (~xi_leb_next_mask))) << shift);
      if ((byte & xi_leb_next_mask) == 0) {
        break;
      }
    }
  }

  if ((source[0] & xi_leb_negative_mask) != 0) {
    *out *= -1;
  }

  if (*out == CHAR_MAX || *out == CHAR_MIN) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_decode_uint8(const xi_byte_t *source, const size_t count, uint8_t *out) {
  *out = 0;
  size_t i = 0;
  for (size_t shift = 0; 1; shift += xi_leb_next_shift) {
    if (i >= count) {
      return XI_VARINT_DECODE_OUT_OF_MEMORY;
    } else if (shift > 8) {
      return XI_VARINT_DECODE_OVERFLOW;
    }
    xi_byte_t byte = source[i++];
    if (byte == 0 && shift != 0) {
      if (i == 2 && source[0] == xi_leb_next_mask) {
        *out = UCHAR_MAX;
        return 2;
      } else {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    const size_t bitsLeft = 8 - shift;
    if (bitsLeft < 8) {
      if (byte & (0xFF << bitsLeft)) {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    *out |= ((uint8_t)((byte & (~xi_leb_next_mask))) << shift);
    if ((byte & xi_leb_next_mask) == 0) {
      break;
    }
  }

  if (*out == UCHAR_MAX) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_decode_int16(const xi_byte_t *source, const size_t count, int16_t *out) {
  if (count < 1) {
    return XI_VARINT_DECODE_OUT_OF_MEMORY;
  }

  xi_byte_t byte = source[0];

  if (byte == xi_leb_negative_mask) {
    *out = SHRT_MIN;
    return 1;
  }

  *out = (int16_t)((byte & (~xi_leb_first_byte_mask)));
  size_t i = 1;

  if ((byte & xi_leb_next_mask) != 0) {
    for (size_t shift = xi_leb_negative_shift; 1; shift += xi_leb_next_shift) {
      if (i >= count) {
        return XI_VARINT_DECODE_OUT_OF_MEMORY;
      } else if (shift > 15) {
        return XI_VARINT_DECODE_OVERFLOW;
      }
      byte = source[i++];
      if (byte == 0) {
        if (i == 2 && source[0] == xi_leb_next_mask) {
          *out = SHRT_MAX;
          return 2;
        } else {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      const size_t bitsLeft = 15 - shift;
      if (bitsLeft < 8) {
        if (byte & (0xFF << bitsLeft)) {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      *out |= ((int16_t)((byte & (~xi_leb_next_mask))) << shift);
      if ((byte & xi_leb_next_mask) == 0) {
        break;
      }
    }
  }

  if ((source[0] & xi_leb_negative_mask) != 0) {
    *out *= -1;
  }

  if (*out == SHRT_MAX || *out == SHRT_MIN) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_decode_uint16(const xi_byte_t *source, const size_t count, uint16_t *out) {
  *out = 0;
  size_t i = 0;
  for (size_t shift = 0; 1; shift += xi_leb_next_shift) {
    if (i >= count) {
      return XI_VARINT_DECODE_OUT_OF_MEMORY;
    } else if (shift > 16) {
      return XI_VARINT_DECODE_OVERFLOW;
    }
    xi_byte_t byte = source[i++];
    if (byte == 0 && shift != 0) {
      if (i == 2 && source[0] == xi_leb_next_mask) {
        *out = USHRT_MAX;
        return 2;
      } else {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    const size_t bitsLeft = 16 - shift;
    if (bitsLeft < 8) {
      if (byte & (0xFF << bitsLeft)) {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    *out |= ((uint16_t)((byte & (~xi_leb_next_mask))) << shift);
    if ((byte & xi_leb_next_mask) == 0) {
      break;
    }
  }

  if (*out == USHRT_MAX) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_decode_int32(const xi_byte_t *source, const size_t count, int32_t *out) {
  if (count < 1) {
    return XI_VARINT_DECODE_OUT_OF_MEMORY;
  }

  xi_byte_t byte = source[0];

  if (byte == xi_leb_negative_mask) {
    *out = INT_MIN;
    return 1;
  }

  *out = (int32_t)((byte & (~xi_leb_first_byte_mask)));
  size_t i = 1;

  if ((byte & xi_leb_next_mask) != 0) {
    for (size_t shift = xi_leb_negative_shift; 1; shift += xi_leb_next_shift) {
      if (i >= count) {
        return XI_VARINT_DECODE_OUT_OF_MEMORY;
      } else if (shift > 31) {
        return XI_VARINT_DECODE_OVERFLOW;
      }
      byte = source[i++];
      if (byte == 0) {
        if (i == 2 && source[0] == xi_leb_next_mask) {
          *out = INT_MAX;
          return 2;
        } else {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      const size_t bitsLeft = 31 - shift;
      if (bitsLeft < 8) {
        if (byte & (0xFF << bitsLeft)) {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      *out |= ((int32_t)((byte & (~xi_leb_next_mask))) << shift);
      if ((byte & xi_leb_next_mask) == 0) {
        break;
      }
    }
  }

  if ((source[0] & xi_leb_negative_mask) != 0) {
    *out *= -1;
  }

  if (*out == INT_MAX || *out == INT_MIN) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_decode_uint32(const xi_byte_t *source, const size_t count, uint32_t *out) {
  *out = 0;
  size_t i = 0;
  for (size_t shift = 0; 1; shift += xi_leb_next_shift) {
    if (i >= count) {
      return XI_VARINT_DECODE_OUT_OF_MEMORY;
    } else if (shift > 32) {
      return XI_VARINT_DECODE_OVERFLOW;
    }
    xi_byte_t byte = source[i++];
    if (byte == 0 && shift != 0) {
      if (i == 2 && source[0] == xi_leb_next_mask) {
        *out = UINT_MAX;
        return 2;
      } else {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    const size_t bitsLeft = 32 - shift;
    if (bitsLeft < 8) {
      if (byte & (0xFF << bitsLeft)) {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    *out |= ((uint32_t)((byte & (~xi_leb_next_mask))) << shift);
    if ((byte & xi_leb_next_mask) == 0) {
      break;
    }
  }

  if (*out == UINT_MAX) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_decode_int64(const xi_byte_t *source, const size_t count, int64_t *out) {
  if (count < 1) {
    return XI_VARINT_DECODE_OUT_OF_MEMORY;
  }

  xi_byte_t byte = source[0];

  if (byte == xi_leb_negative_mask) {
    *out = LLONG_MIN;
    return 1;
  }
  *out = (int64_t)((byte & (~xi_leb_first_byte_mask)));
  size_t i = 1;

  if ((byte & xi_leb_next_mask) != 0) {
    for (size_t shift = xi_leb_negative_shift; 1; shift += xi_leb_next_shift) {
      if (i >= count) {
        return XI_VARINT_DECODE_OUT_OF_MEMORY;
      } else if (shift > 63) {
        return XI_VARINT_DECODE_OVERFLOW;
      }
      byte = source[i++];
      if (byte == 0) {
        if (i == 2 && source[0] == xi_leb_next_mask) {
          *out = LLONG_MAX;
          return 2;
        } else {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      const size_t bitsLeft = 63 - shift;
      if (bitsLeft < 8) {
        if (byte & (0xFF << bitsLeft)) {
          return XI_VARINT_DECODE_NONE_CANONICAL;
        }
      }
      *out |= ((int64_t)((byte & (~xi_leb_next_mask))) << shift);
      if ((byte & xi_leb_next_mask) == 0) {
        break;
      }
    }
  }

  if ((source[0] & xi_leb_negative_mask) != 0) {
    *out *= -1;
  }

  if (*out == LLONG_MAX || *out == LLONG_MIN) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_decode_uint64(const xi_byte_t *source, const size_t count, uint64_t *out) {
  *out = 0;
  size_t i = 0;
  for (size_t shift = 0; 1; shift += xi_leb_next_shift) {
    if (i >= count) {
      return XI_VARINT_DECODE_OUT_OF_MEMORY;
    } else if (shift > 64) {
      return XI_VARINT_DECODE_OVERFLOW;
    }
    xi_byte_t byte = source[i++];

    if (byte == 0 && shift != 0) {
      if (i == 2 && source[0] == xi_leb_next_mask) {
        *out = ULLONG_MAX;
        return 2;
      } else {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    const size_t bitsLeft = 64 - shift;
    if (bitsLeft < 8) {
      if (byte & (0xFF << bitsLeft)) {
        return XI_VARINT_DECODE_NONE_CANONICAL;
      }
    }
    *out |= ((uint64_t)((byte & (~xi_leb_next_mask))) << shift);
    if ((byte & xi_leb_next_mask) == 0) {
      break;
    }
  }

  if (*out == ULLONG_MAX) {
    return XI_VARINT_DECODE_NONE_CANONICAL;
  }
  return i;
}

size_t xi_encoding_varint_encode_int8(int8_t value, xi_byte_t *dest, const size_t count) {
  if (count < 1) {
    return XI_VARINT_ENCODE_OUT_OF_MEMORY;
  }

  if (value == CHAR_MIN) {
    dest[0] = xi_leb_negative_mask;
    return 1;
  } else if (value == CHAR_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  if (value < 0) {
    dest[0] = xi_leb_negative_mask;
    value *= -1;
  } else {
    dest[0] = 0;
  }

  size_t i = 0;
  if (value >= xi_leb_negative_mask) {
    dest[i++] |= (((xi_byte_t)value) & (~xi_leb_first_byte_mask)) | xi_leb_next_mask;
    value >>= xi_leb_negative_shift;
  } else {
    dest[i++] |= ((xi_byte_t)value);
    return i;
  }

  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

size_t xi_encoding_varint_encode_uint8(uint8_t value, xi_byte_t *dest, const size_t count) {
  if (value == UCHAR_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  size_t i = 0;
  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

size_t xi_encoding_varint_encode_int16(int16_t value, xi_byte_t *dest, const size_t count) {
  if (count < 1) {
    return XI_VARINT_ENCODE_OUT_OF_MEMORY;
  }

  if (value == SHRT_MIN) {
    dest[0] = xi_leb_negative_mask;
    return 1;
  } else if (value == SHRT_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  if (value < 0) {
    dest[0] = xi_leb_negative_mask;
    value *= -1;
  } else {
    dest[0] = 0;
  }

  size_t i = 0;
  if (value >= xi_leb_negative_mask) {
    dest[i++] |= (((xi_byte_t)value) & (~xi_leb_first_byte_mask)) | xi_leb_next_mask;
    value >>= xi_leb_negative_shift;
  } else {
    dest[i++] |= ((xi_byte_t)value);
    return i;
  }

  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

size_t xi_encoding_varint_encode_uint16(uint16_t value, xi_byte_t *dest, const size_t count) {
  if (value == USHRT_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  size_t i = 0;
  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

size_t xi_encoding_varint_encode_int32(int32_t value, xi_byte_t *dest, const size_t count) {
  if (count < 1) {
    return XI_VARINT_ENCODE_OUT_OF_MEMORY;
  }

  if (value == INT_MIN) {
    dest[0] = xi_leb_negative_mask;
    return 1;
  } else if (value == INT_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  if (value < 0) {
    dest[0] = xi_leb_negative_mask;
    value *= -1;
  } else {
    dest[0] = 0;
  }

  size_t i = 0;
  if (value >= xi_leb_negative_mask) {
    dest[i++] |= (((xi_byte_t)value) & (~xi_leb_first_byte_mask)) | xi_leb_next_mask;
    value >>= xi_leb_negative_shift;
  } else {
    dest[i++] |= ((xi_byte_t)value);
    return i;
  }

  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

size_t xi_encoding_varint_encode_uint32(uint32_t value, xi_byte_t *dest, const size_t count) {
  if (value == UINT_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  size_t i = 0;
  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

size_t xi_encoding_varint_encode_int64(int64_t value, xi_byte_t *dest, const size_t count) {
  if (count < 1) {
    return XI_VARINT_ENCODE_OUT_OF_MEMORY;
  }

  if (value == LLONG_MIN) {
    dest[0] = xi_leb_negative_mask;
    return 1;
  } else if (value == LLONG_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  if (value < 0) {
    dest[0] = xi_leb_negative_mask;
    value *= -1;
  } else {
    dest[0] = 0;
  }

  size_t i = 0;
  if (value >= xi_leb_negative_mask) {
    dest[i++] |= (((xi_byte_t)value) & (~xi_leb_first_byte_mask)) | xi_leb_next_mask;
    value >>= xi_leb_negative_shift;
  } else {
    dest[i++] |= ((xi_byte_t)value);
    return i;
  }

  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

size_t xi_encoding_varint_encode_uint64(uint64_t value, xi_byte_t *dest, const size_t count) {
  if (value == ULLONG_MAX) {
    if (count < 2) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    } else {
      dest[0] = xi_leb_next_mask;
      dest[1] = 0;
      return 2;
    }
  }

  size_t i = 0;
  while (value >= xi_leb_next_mask) {
    if (i + 1 >= count) {
      return XI_VARINT_ENCODE_OUT_OF_MEMORY;
    }
    dest[i++] = ((xi_byte_t)value) | xi_leb_next_mask;
    value >>= xi_leb_next_shift;
  }
  dest[i++] = (xi_byte_t)value;
  return i;
}

int xi_encoding_varint_has_successor(xi_byte_t current) {
  if ((current & xi_leb_next_mask) == 0) {
    return XI_FALSE;
  } else {
    return XI_TRUE;
  }
}
