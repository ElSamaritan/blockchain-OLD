#include "crypto/cnx/randomizer.h"

#include <cmath>

Crypto::CNX::Randomizer::Randomizer(uint32_t maxSize)
{
  Handle.size = 0;

  _Operators.resize(maxSize);
  Handle.operators = &_Operators[0];

  _Indices.resize(maxSize);
  Handle.indices = &_Indices[0];

  _Values.resize(maxSize);
  Handle.values = &_Values[0];
}

Crypto::CNX::Randomizer::~Randomizer()
{
  /* */
}

void Crypto::CNX::Randomizer::reset(uint32_t size)
{
  Handle.size = size;
  std::memset(_Operators.data(), 0, Handle.size);
  std::memset(_Indices.data(), 0, Handle.size * sizeof(uint32_t));
  std::memset(_Values.data(), 0, Handle.size);
}

uint32_t Crypto::CNX::Randomizer::size() const
{
  return Handle.size;
}
