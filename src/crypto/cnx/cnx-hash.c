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

#include "crypto/cnx/cnx-hash.h"

#include <stddef.h>
#include <stdio.h>
#include <errno.h>

#include "crypto/hash-predef.h"
#include "crypto/hash-allocation.h"
#include "crypto/hash-emmintrin.h"
#include "crypto/hash-extra-ops.h"
#include "crypto/oaes_lib.h"
#include "crypto/keccak.h"
#include "crypto/__impl/cn-aes.h"

#define INIT_SIZE_BLK 8
#define INIT_SIZE_BYTE (INIT_SIZE_BLK * AES_BLOCK_SIZE)
#define U64(x) ((uint64_t *)(x))

STATIC THREADV oaes_ctx* __oaes;

STATIC INLINE void __cnx_xor_blocks(uint8_t *a, const uint8_t *b) {
  U64(a)[0] ^= ((const uint64_t *)(b))[0];
  U64(a)[1] ^= ((const uint64_t *)(b))[1];
}

typedef union __hash_state __hash_state;
#pragma pack(push, 1)
union __hash_state {
  uint8_t b[200];
  uint64_t w[25];
};
#pragma pack(pop)

typedef union __cnx_hash_state __cnx_hash_state;
#pragma pack(push, 1)
union __cnx_hash_state {
  __hash_state hs;
  struct {
    uint8_t k[64];
    uint8_t init[INIT_SIZE_BYTE];
  } data;
};
#pragma pack(pop)

/*!
 * About encoded addresses.
 *
 * The RAM can have a maximum size of 2MB. With a 16bit stride we must be able to address 1024 * 1024 different
 * addresses yielding LOG_2(1024 * 1024) = 20. Thus we require 20bits to encode an address. This is not possible
 * with 16bit address space. Since we are not generally required to do something meaningfull the addresses are
 * encoded using references to registers. Therefore the referenced registers 1 and 2 are used to build a 32bit
 * integer which yield the address.
 *
 * IE. In a LOAD operation REG0 is the register address that will store the loaded value from memory and REG1 + REG2
 * encode the address to be loaded.
 */

enum {
  // Address Based Instructions
  // If an address instruction is encountered the referenced registries are reinterpreted as memory location to
  // obtain an address.
  LOAD    =  0,  ///< Loads the encoded memory address into the index register REG0
  STORE   =  1,  ///< Stores REG0 into the memory address encoded by REG1 and REG2
  JMP     = 13,  ///< Sets the program counter to the given address
  JIN     = 14,  ///< Sets the program counter to the given address if the compare flag is not set in REG0
  JIE     = 15,  ///< Sets the program counter to the given address if the compare flag is set in REG1

  // Value Based Instructions
  ADD     =  2,  ///< Adds the encoded value into the indexed register REG0
  SUB     =  3,  ///< Subtracts the encoded value  into the indexed register REG0
  MUL     =  4,  ///< Multiplies the encoded value  into the indexed register REG0
  DIV     =  5,  ///< Divides the encoded value into the indexed register REG0, if the  value is zero the operation is
                 ///< is skipped.

  // Binary Register Functions
  EQ      =  6,  ///< Copies REG1 into REG0
  NEG     =  7,  ///< Stores the negation of REG1 into REG0
  AND     =  8,  ///< Stores REG1 & REG2 into REG0
  OR      =  9,  ///< Stores REG1 | REG2 into REG0
  XOR     = 10,  ///< Stores REG1 ^ REG2 into REG0
  LESS    = 11,  ///< Sets the CMP_FLAG on REG0 if REG1 < REG2 otherwise clears it.
  GREATER = 12,  ///< Sets the CMP_FLAG on REG1 if REG1 > REG2 otherwise clears it.
};

enum {
  REG_NUM = 16
};

static const uint16_t CNX_INST_MASK      = 0x000F; ///< Instruction to perform
static const uint16_t CNX_VAL_MASK       = 0xFFF0; ///< Encoded value to apply
static const uint16_t CNX_REG1_MASK      = 0x00F0; ///< First register operand index
static const uint16_t CNX_REG2_MASK      = 0x0F00; ///< Second register operand index
static const uint16_t CNX_REGL_MASK      = 0xF000; ///< register operand index for loading
static const uint16_t CNX_REG_CMP_FLAG   = 0x8000; ///< Flag to store comparison result in the registers

typedef union __cnx_cpu_register __cnx_cpu_register;
union __cnx_cpu_register {
  uint16_t b16[REG_NUM];
  uint64_t b64[REG_NUM / 4];
};

typedef struct __cnx_cpu_state __cnx_cpu_state;
struct __cnx_cpu_state {
  RDATA_ALIGN16 __cnx_cpu_register registers;
  uint16_t* ram;
  uint32_t ram_size;
  uint32_t program_counter;
  uint8_t accumulator_index;
};

STATIC INLINE uint8_t __cnx_cpu_get_instruction(uint16_t value) { return (uint8_t)(value & CNX_INST_MASK); }
STATIC INLINE uint16_t __cnx_cpu_get_value(uint16_t value) { return (uint16_t)(value & CNX_VAL_MASK) >> 4; }
STATIC INLINE uint32_t __cnx_cpu_get_cmp_flag(uint16_t value) { return (uint16_t)(value & CNX_REG_CMP_FLAG); }

STATIC INLINE uint8_t __cnx_cpu_get_register_index_1(uint16_t value) { return (uint8_t)((value & CNX_REG1_MASK) >>  4); }
STATIC INLINE uint8_t __cnx_cpu_get_register_index_2(uint16_t value) { return (uint8_t)((value & CNX_REG2_MASK) >>  8); }
STATIC INLINE uint8_t __cnx_cpu_get_register_index_l(uint16_t value) { return (uint8_t)((value & CNX_REGL_MASK) >> 12); }

STATIC INLINE uint32_t __cnx_cpu_translate_address(const __cnx_cpu_state* cpu, uint16_t value, uint32_t ram_size) {
  const uint32_t addr = (((uint32_t)cpu->registers.b16[__cnx_cpu_get_register_index_1(value)]) << 16)
                     |  (((uint32_t)cpu->registers.b16[__cnx_cpu_get_register_index_2(value)]) <<  0);
  return addr % ram_size; // TODO find a way to make it at least less biased...
}

STATIC INLINE uint8_t __cnx_cpu_is_not_jump_instruction(uint16_t value) {
  switch(__cnx_cpu_get_instruction(value)) {
    case JMP:
    case JIN:
    case JIE:
      return 0;

    default:
      return 1;
  }
}

STATIC INLINE void __cnx_cpu_address_instruction(__cnx_cpu_state* cpu, const uint8_t instruction, uint16_t value) {
  switch (instruction) {
    case LOAD:
      cpu->registers.b16[__cnx_cpu_get_register_index_l(value)] = cpu->ram[__cnx_cpu_translate_address(cpu, value, cpu->ram_size)];
      break;

    case STORE:
      cpu->ram[__cnx_cpu_translate_address(cpu, value, cpu->ram_size)] = cpu->registers.b16[cpu->accumulator_index];
      break;

    case JMP:
      cpu->program_counter = cpu->ram[__cnx_cpu_translate_address(cpu, value, cpu->ram_size)];
      break;

    case JIN:
      if(__cnx_cpu_get_cmp_flag(cpu->registers.b16[cpu->accumulator_index]) == 0)
        cpu->program_counter = cpu->ram[__cnx_cpu_translate_address(cpu, value, cpu->ram_size)];
      break;

    case JIE:
      if(__cnx_cpu_get_cmp_flag(cpu->registers.b16[cpu->accumulator_index]))
        cpu->program_counter = cpu->ram[__cnx_cpu_translate_address(cpu, value, cpu->ram_size)];
      break;
  }
}

STATIC INLINE void __cnx_cpu_value_instruction(__cnx_cpu_state* cpu, const uint8_t instruction, uint16_t value) {
  const uint8_t registerIndex = cpu->accumulator_index;
  const uint16_t encValue = __cnx_cpu_get_value(value);
  switch(instruction) {
    case ADD:
      cpu->registers.b16[registerIndex] += encValue;
      break;

    case SUB:
      cpu->registers.b16[registerIndex] -= encValue;
      break;

    case MUL:
      cpu->registers.b16[registerIndex] *= encValue;
      break;

    case DIV:
      if(encValue) cpu->registers.b16[registerIndex] /= encValue;
      break;
  }
}

STATIC INLINE void __cnx_cpu_register_instruction(__cnx_cpu_state* cpu, const uint8_t instruction, uint16_t value) {
  const uint8_t op_a_index = __cnx_cpu_get_register_index_1(value);
  const uint8_t op_b_index = __cnx_cpu_get_register_index_2(value);

  switch(instruction) {
    case(NEG):
      cpu->registers.b16[cpu->accumulator_index] = ~cpu->registers.b16[op_a_index];
      break;

    case(EQ):
      cpu->registers.b16[cpu->accumulator_index] = cpu->registers.b16[op_a_index];
      break;

    case(AND):
      cpu->registers.b16[cpu->accumulator_index] = cpu->registers.b16[op_a_index] & cpu->registers.b16[op_b_index];
      break;

    case(OR):
      cpu->registers.b16[cpu->accumulator_index] = cpu->registers.b16[op_a_index] | cpu->registers.b16[op_b_index];
      break;

    case(XOR):
      cpu->registers.b16[cpu->accumulator_index] = cpu->registers.b16[op_a_index] ^ cpu->registers.b16[op_b_index];
      break;

    case(LESS):
      if(cpu->registers.b16[op_a_index] < cpu->registers.b16[op_b_index])
        cpu->registers.b16[cpu->accumulator_index] |= CNX_REG_CMP_FLAG;
      else
        cpu->registers.b16[cpu->accumulator_index] |= ~CNX_REG_CMP_FLAG;
      break;

    case(GREATER):
      if(cpu->registers.b16[op_a_index] > cpu->registers.b16[op_b_index])
        cpu->registers.b16[cpu->accumulator_index] |= CNX_REG_CMP_FLAG;
      else
        cpu->registers.b16[cpu->accumulator_index] |= ~CNX_REG_CMP_FLAG;
      break;

  }
}

STATIC INLINE void __cnx_cpu_tick(__cnx_cpu_state* cpu) {
  cpu->program_counter = (cpu->program_counter + 1) % cpu->ram_size;
  const uint16_t value = cpu->ram[cpu->program_counter];

  const uint8_t instruction = __cnx_cpu_get_instruction(value);
  switch (instruction) {
    case LOAD:
    case STORE:
    case JMP:
    case JIN:
    case JIE:
      __cnx_cpu_address_instruction(cpu, instruction, value);
      break;

    case ADD:
    case SUB:
    case MUL:
    case DIV:
      __cnx_cpu_value_instruction(cpu, instruction, value);
      break;

    case(NEG):
    case(EQ):
    case(AND):
    case(OR):
    case(XOR):
    case(LESS):
    case(GREATER):
      __cnx_cpu_register_instruction(cpu, instruction, value);
      break;
  }
}

#if defined(_MSC_VER)
#if !defined(_WIN64)
#define MUL() low = mul128(buffer[0], cpu.registers.b16[8], &high);
#else
#define MUL() low = _umul128(buffer[0], cpu.registers.b16[8], &high);
#endif
#else
#if defined(__x86_64__)
STATIC INLINE uint64_t _umul128(const uint64_t left, const uint64_t right, uint64_t* high)
{
  // cppcheck-suppress uninitvar
  uint64_t low;
  ASM("mulq %3\n\t" : "=d"(*high), "=a"(low) : "%a"(left), "rm"(right) : "cc");
  return low;
}
#define MUL() low = _umul128(buffer[0], cpu.registers.b16[8], &high);
#else
#define MUL() low = mul128(buffer[0], cpu.registers.b16[8], &high);
#endif
#endif

#define PRE_AES()                                                                                 \
  scratchpad_index = __cnx_cpu_translate_address(&cpu, cpu.registers.b16[cpu.accumulator_index],  \
      scratchpad_size / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;                                         \
  _3 = _mm_load_si128(R128(&scratchpad[scratchpad_index]));                                       \
  _1 = _mm_load_si128(R128(&cpu.registers.b16[0]))

#define POST_AES()                                            \
  _mm_store_si128(R128(buffer), _3); \
  scratchpad_pointer = U64(&scratchpad[scratchpad_index]); \
  _mm_store_si128(R128(scratchpad_pointer), _mm_xor_si128(_2, _3)); \
  cpu.registers.b64[0] ^= scratchpad_pointer[0]; \
  cpu.registers.b64[1] = scratchpad_pointer[1]; \
  cpu.registers.b64[2] = scratchpad_pointer[0]; \
  cpu.registers.b64[3] ^= scratchpad_pointer[1]; \
  scratchpad_index = __cnx_cpu_translate_address(&cpu, cpu.registers.b16[cpu.accumulator_index], \
      scratchpad_size / AES_KEY_SIZE) * AES_KEY_SIZE; \
  scratchpad_pointer = U64(&scratchpad[scratchpad_index]); \
  cpu.registers.b64[2] = scratchpad_pointer[0]; \
  cpu.registers.b64[3] = scratchpad_pointer[1]; \
  MUL(); \
  cpu.registers.b64[0] += high; \
  cpu.registers.b64[1] += low; \
  scratchpad_pointer = U64(&scratchpad[scratchpad_index]); \
  scratchpad_pointer[0] = cpu.registers.b64[0]; \
  scratchpad_pointer[1] = cpu.registers.b64[1]; \
  cpu.registers.b64[0] ^= cpu.registers.b64[2]; \
  cpu.registers.b64[1] ^= cpu.registers.b64[3]; \
  _2 = _3

void cnx_hash(const uint8_t *data, const size_t length, const cnx_hash_config *config, uint8_t *hash)
{
  uint8_t* scratchpad = xi_hash_allocate_state(config->scratchpad_size);
  uint32_t scratchpad_size = config->scratchpad_size;
  uint8_t use_hw_aes = (config->flags & CNX_FLAGS_HARDWARE_AES);
  if(use_hw_aes == 0) __oaes = (oaes_ctx *)oaes_alloc();

  uint32_t scratchpad_index;
  uint64_t *scratchpad_pointer = NULL;
  __cnx_cpu_state cpu;
  uint8_t text[INIT_SIZE_BYTE];
  RDATA_ALIGN16 __cnx_hash_state state;
  RDATA_ALIGN16 uint8_t expandedKey[240];
  RDATA_ALIGN16 uint64_t buffer[4];
  RDATA_ALIGN16 uint64_t high, low;
  RDATA_ALIGN16 __m128i _1, _2, _3;

  /* CryptoNight Step 1:  Use Keccak1600 to initialize the 'state' (and 'text') buffers from the data. */
  keccak1600(data, (int)length, (uint8_t*)&state.hs);
  memcpy(text, state.data.init, INIT_SIZE_BYTE);
  for(uint8_t i = 0; i < INIT_SIZE_BYTE; ++i) {
    text[i] ^= config->salt[i % config->salt_size];
  }

  /* CryptoNight Step 2:  Iteratively encrypt the results from Keccak to fill scratchpad */
  const uint32_t init_rounds = scratchpad_size / INIT_SIZE_BYTE;
  if (use_hw_aes) {
    aes_expand_key(state.hs.b, expandedKey);
    for (uint32_t i = 0; i < init_rounds; i++) {
      aes_pseudo_round(text, text, expandedKey, INIT_SIZE_BLK);
      memcpy(&scratchpad[i * INIT_SIZE_BYTE], text, INIT_SIZE_BYTE);
    }
  } else {
    oaes_key_import_data(__oaes, state.hs.b, AES_KEY_SIZE);
    for (uint32_t i = 0; i < init_rounds; i++) {
      for (uint8_t j = 0; j < INIT_SIZE_BLK; j++) {
        aesb_pseudo_round(&text[AES_BLOCK_SIZE * j], &text[AES_BLOCK_SIZE * j], __oaes->key->exp_data);
      }
      memcpy(&scratchpad[i * INIT_SIZE_BYTE], text, INIT_SIZE_BYTE);
    }
  }

  cpu.ram = (uint16_t*)scratchpad;
  cpu.ram_size = scratchpad_size / 2;
  cpu.accumulator_index = 0;
  cpu.program_counter = 0;

  cpu.registers.b64[0] = U64(&state.data.k[0])[0] ^ U64(&state.data.k[32])[0];
  cpu.registers.b64[1] = U64(&state.data.k[0])[1] ^ U64(&state.data.k[32])[1];
  cpu.registers.b64[2] = U64(&state.data.k[16])[0] ^ U64(&state.data.k[48])[0];
  cpu.registers.b64[3] = U64(&state.data.k[16])[1] ^ U64(&state.data.k[48])[1];
  _2 = _mm_load_si128(R128(&cpu.registers.b64[2]));

  if(use_hw_aes > 0){
    for(uint32_t i = 0; i < config->iterations / REG_NUM; ++i) {
      for(uint8_t j = 0; j < REG_NUM; ++j) {
        cpu.accumulator_index = j;
        __cnx_cpu_tick(&cpu);
        if(__cnx_cpu_get_cmp_flag(cpu.registers.b16[j])) {
          PRE_AES();
          _3 = _mm_aesenc_si128(_3, _1);
          POST_AES();
        }
      }
      PRE_AES();
      _3 = _mm_aesenc_si128(_3, _1);
      POST_AES();
    }
  } else {
    for(uint32_t i = 0; i < config->iterations / REG_NUM; ++i) {
      for(uint8_t j = 0; j < REG_NUM; ++j) {
        cpu.accumulator_index = j;
        __cnx_cpu_tick(&cpu);
        if(__cnx_cpu_get_cmp_flag(cpu.registers.b16[j])) {
          PRE_AES();
          aesb_single_round((uint8_t *)&_3, (uint8_t *)&_3, (uint8_t *)&_1);
          POST_AES();
        }
      }
      PRE_AES();
      aesb_single_round((uint8_t *)&_3, (uint8_t *)&_3, (uint8_t *)&_1);
      POST_AES();
    }
  }

  /*
   * CryptoNight Step 4:  Sequentially pass through the mixing buffer and use 10 rounds
   * of AES encryption to mix the random data back into the 'text' buffer.  'text'
   * was originally created with the output of Keccak1600.
   */
  memcpy(text, state.data.init, INIT_SIZE_BYTE);
  if (use_hw_aes) {
   aes_expand_key(&state.hs.b[32], expandedKey);
   for (uint32_t i = 0; i < init_rounds; i++) {
     // add the xor to the pseudo round
     aes_pseudo_round_xor(text, text, expandedKey, &scratchpad[i * INIT_SIZE_BYTE], INIT_SIZE_BLK);
   }
  } else {
    oaes_key_import_data(__oaes, &state.hs.b[32], AES_KEY_SIZE);
    for (uint32_t i = 0; i < init_rounds; i++) {
      for (uint8_t j = 0; j < INIT_SIZE_BLK; j++) {
        __cnx_xor_blocks(&text[j * AES_BLOCK_SIZE], &scratchpad[i * INIT_SIZE_BYTE + j * AES_BLOCK_SIZE]);
        aesb_pseudo_round(&text[AES_BLOCK_SIZE * j], &text[AES_BLOCK_SIZE * j], __oaes->key->exp_data);
      }
    }
  }


  /*
   * CryptoNight Step 5:  Apply Keccak to the state again, and then
   * use the resulting data to select which of four finalizer
   * hash functions to apply to the data (Blake, Groestl, JH, or Skein).
   * Use this hash to squeeze the state array down
   * to the final 256 bit hash output.
   */
  memcpy(state.data.init, text, INIT_SIZE_BYTE);
  keccakf((uint64_t *)&state.hs, 24);
  hash_extra_functions[state.hs.b[0] & 3](&state, 200, (char*)hash);

  xi_hash_free_state();
  scratchpad = NULL;
  scratchpad_size = 0;
  if(use_hw_aes == 0) oaes_free((OAES_CTX **)(&__oaes));
  use_hw_aes = 0;
}
