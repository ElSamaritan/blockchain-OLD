#include <iostream>
#include <string>

#include <crypto/cnx/cnx.h>
#include <Common/StringTools.h>

std::string makeHash(std::string data, uint32_t height) {
  Crypto::CNX::Hash_v0 hashFn;
  Crypto::Hash hash;
  hashFn(data.c_str(), data.length(), hash, height);
  std::string hashString;
  hashString.resize(sizeof (hash));
  for(std::size_t i = 0 ; i < sizeof (hash); ++i)
    hashString[i] = reinterpret_cast<char*>(&hash)[i];
  return Common::base64Encode(hashString);
}

int main(int, char**) {
  std::string currentLine;
  while(!std::cin.eof()) {
    std::getline(std::cin, currentLine, '\n');
    if(currentLine.empty()) continue;
    std::string data = Common::base64Decode(currentLine);
    for(uint32_t i = 0; i < Crypto::CNX::Hash_v0::windowSize() * 2 + 1; i += 512)
      std::cout << makeHash(data, i) << std::endl;
  }
}
