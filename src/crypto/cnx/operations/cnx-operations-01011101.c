#include "crypto/cnx/operations/cnx-operations-01011101.h"

void CN_ADAPTIVE_OP_01011101_00000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  (void)inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_00111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_01111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_10111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_01011101_11111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

