#include "crypto/cnx/operations/cnx-operations-00100111.h"

void CN_ADAPTIVE_OP_00100111_00000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  (void)inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_00111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_01111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_10111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_00100111_11111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

