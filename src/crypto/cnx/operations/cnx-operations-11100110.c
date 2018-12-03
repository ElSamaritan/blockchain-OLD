#include "crypto/cnx/operations/cnx-operations-11100110.h"

void CN_ADAPTIVE_OP_11100110_00000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  (void)inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_00111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_01111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_10111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11000111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11001111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11010111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11011111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11100111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand += *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11101111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11110111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111000(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111001(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111010(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111011(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111100(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111101(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111110(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
}

void CN_ADAPTIVE_OP_11100110_11111111(uint8_t* inPlaceOperand, const uint8_t* appliedOperand) {
  *inPlaceOperand &= *appliedOperand;
  *inPlaceOperand += *appliedOperand;
  *inPlaceOperand |= *appliedOperand;
  *inPlaceOperand -= *appliedOperand;
  *inPlaceOperand = ~*inPlaceOperand;
  (void)appliedOperand;
  *inPlaceOperand ^= *appliedOperand;
  *inPlaceOperand = ~*appliedOperand;
  *inPlaceOperand = *appliedOperand;
}

