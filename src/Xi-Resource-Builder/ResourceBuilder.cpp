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

#include <iostream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Exceptions.hpp>
#include <Xi/FileSystem.h>
#include <Xi/Algorithm/String.h>
#include <Common/StringTools.h>
#include <Xi/Resource/Resource.hpp>
#include <Xi/Crypto/FastHash.hpp>
#include <Xi/Crypto/Random/Random.hh>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/YamlInputSerializer.hpp>

namespace XiResourceBuilder {

const std::string Copyright = R"__(
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
)__";

const std::string Header = R"__(
namespace {
)__";

const std::string Footer = R"__(
}
)__";

struct Options {
  std::vector<std::string> inputs;
  std::string output;
};

struct ResourceEntity {
  std::string name;
  std::string path{};
  Xi::Resource::Type type{};

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(name)
  KV_MEMBER(path)
  KV_MEMBER(type)
  KV_END_SERIALIZATION
};

struct ResourceDefinition {
  std::string _namespace;
  std::string identifier;
  std::string prefix{};
  std::string root{};
  std::vector<ResourceEntity> entities{};

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(_namespace, namespace)
  KV_MEMBER(identifier)
  KV_MEMBER(prefix)
  KV_MEMBER(root)
  KV_MEMBER(entities)
  KV_END_SERIALIZATION
};

std::string entityIdentifier(const std::string& path) {
  return std::string{"__resource_"} + Xi::Crypto::FastHash::compute(Xi::asConstByteSpan(path)).takeOrThrow().toString();
}

Xi::Crypto::FastHash resourceHash(const ResourceDefinition& resources, const std::string& input) {
  Xi::Crypto::FastHashHashVector hashes{};
  hashes.reserve(1 + resources.entities.size());
  hashes.emplace_back(Xi::Crypto::FastHash::computeObjectHash(resources).takeOrThrow());
  for (const auto& entity : resources.entities) {
    const auto inputFileDir = Xi::FileSystem::directory(input).takeOrThrow();
    const auto collectionDir = Xi::FileSystem::rooted(resources.root, inputFileDir).takeOrThrow();
    const auto entityPath = Xi::FileSystem::rooted(entity.path, collectionDir).takeOrThrow();
    const auto binary = Xi::FileSystem::readBinaryFile(entityPath).takeOrThrow();
    hashes.emplace_back(Xi::Crypto::FastHash::compute(binary).takeOrThrow());
  }
  return Xi::Crypto::FastHash::computeMerkleTree(hashes).takeOrThrow();
}

std::string wrapText(const std::string& text, const std::string& id) {
  std::stringstream builder{};
  builder << "static const std::string " << id << " = [](){\n";
  builder << R"_(
  std::stringstream builder{};
)_";
  const size_t StringLengthLimit = 16380 - 64;
  for (size_t i = 0; i * StringLengthLimit < text.size(); ++i) {
    const size_t offset = i * StringLengthLimit;
    const size_t count = std::min(StringLengthLimit, text.size() - offset);
    std::string itext = text.substr(offset, count);

    std::string seperator{};
    do {
      Xi::ByteArray<8> seperatorBytes{};
      (void)Xi::Crypto::Random::generate(seperatorBytes);
      seperator = Common::toHex(seperatorBytes.data(), seperatorBytes.size());
    } while (itext.find(seperator) != std::string::npos);
    builder << "\nbuilder << R\"" << seperator << "(";
    builder << itext;
    builder << ")" << seperator << "\";\n";
  }
  builder << "\nreturn builder.str();\n}();";
  return builder.str();
}

}  // namespace XiResourceBuilder

int main(int argc, char** argv) {
  using namespace Xi;
  using namespace Xi::Resource;
  using namespace XiResourceBuilder;

  try {
    Options options{};
    cxxopts::Options cli{"Xi.Resource.Builder", "Generates source files to embed resources into the application."};
    // clang-format off
    cli.add_options("")
      ("i,inputs", "the file to embed", cxxopts::value(options.inputs))
      ("o,output", "the file to embed", cxxopts::value(options.output))
    ;
    // clang-format on
    cli.parse(argc, argv);

    for (const auto& input : options.inputs) {
      ResourceDefinition resources{};
      if (!CryptoNote::fromYaml(FileSystem::readTextFile(input).takeOrThrow(), resources)) {
        exceptional<RuntimeError>("Faile to parse input file: {}", input);
      }

      const auto ns = split(resources._namespace, ":");

      const auto cacheHash = resourceHash(resources, input);
      const auto cachePath = FileSystem::combine(options.output, resources.identifier + ".cache").takeOrThrow();
      if (FileSystem::exists(cachePath).takeOrThrow()) {
        const auto previousCacheHashString = FileSystem::readTextFile(cachePath).takeOrThrow();
        const auto previousCacheHash = Xi::Crypto::FastHash::fromString(previousCacheHashString);
        if (!previousCacheHash.isError() && cacheHash == *previousCacheHash) {
          continue;
        }
      }

      const auto includeDir = FileSystem::combine(options.output, "/include").takeOrThrow();
      const auto headersDir = FileSystem::combine(includeDir, join(ns, "/")).takeOrThrow();
      FileSystem::ensureDirectoryExists(headersDir).throwOnError();
      const auto headerPath = FileSystem::combine(headersDir, resources.identifier + ".hpp").takeOrThrow();

      const auto sourceDir = FileSystem::combine(options.output, "/source").takeOrThrow();
      FileSystem::ensureDirectoryExists(sourceDir).throwOnError();
      const auto sourcePath = FileSystem::combine(sourceDir, resources.identifier + ".cpp").takeOrThrow();

      std::ofstream headerBuilder{headerPath, std::ios::trunc};
      std::ofstream sourceBuilder{sourcePath, std::ios::trunc};

      headerBuilder << Copyright << "\n#pragma once\n";
      sourceBuilder << Copyright << "\n";
      sourceBuilder << "#include \"" << join(ns, "/") << "/" << resources.identifier + ".hpp\"\n\n";
      sourceBuilder << "#include <mutex>\n"
                    << "#include <atomic>\n"
                    << "#include <thread>\n\n"
                    << "#include <sstream>\n\n";
      sourceBuilder << "#include <Xi/Resource/Resource.hpp>\n\n";
      for (const auto& ins : ns) {
        headerBuilder << "namespace " << ins << " {\n";
        sourceBuilder << "namespace " << ins << " {\n";
      }

      headerBuilder << "\nvoid load" << resources.identifier << "();\n";
      sourceBuilder << "\n";

      std::stringstream initBuilder{};
      initBuilder << "void load" << resources.identifier << "() {\n"
                  << R"__(
  static std::mutex __guard{};
  static std::atomic_bool __isInitialized{false};

  std::lock_guard<std::mutex> lock{__guard};
  (void)lock;

  if(__isInitialized) {
    return;
  }

  auto& container = const_cast<::Xi::Resource::Container&>(::Xi::Resource::embedded());

)__";

      const auto inputFileDir = FileSystem::directory(input).takeOrThrow();
      const auto collectionDir = FileSystem::rooted(resources.root, inputFileDir).takeOrThrow();
      for (const auto& entity : resources.entities) {
        const auto entityPath = FileSystem::rooted(entity.path, collectionDir).takeOrThrow();
        const auto enitityId = resourcePrefix() + resources.prefix + entity.name;
        const auto entityString = entityIdentifier(enitityId);

        if (entity.type == Type::Text) {
          const auto text = FileSystem::readTextFile(entityPath).takeOrThrow();
          sourceBuilder << wrapText(text, entityString) << std::endl;
        }
        initBuilder << "  container.set(\"" << enitityId << "\", " << entityString << ").throwOnError();\n";
      }

      initBuilder << "}\n";
      sourceBuilder << "\n" << initBuilder.str();

      headerBuilder << "\n";
      sourceBuilder << "\n";

      for (auto i = ns.rbegin(); i != ns.rend(); ++i) {
        headerBuilder << "} // " << *i << "\n";
        sourceBuilder << "} // " << *i << "\n";
      }

      FileSystem::writeTextFile(cachePath, cacheHash.toString(), true).throwOnError();
    }

    return EXIT_SUCCESS;
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
