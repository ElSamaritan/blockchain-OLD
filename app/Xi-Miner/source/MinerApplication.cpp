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

#include <numeric>

#include <Xi/App/Application.h>
#include <Common/SignalHandler.h>

#include "MinerOptions.h"
#include "UpdateMonitor.h"
#include "MinerManager.h"
#include "MinerCommandsHandler.h"

using namespace Xi::App;

namespace XiMiner {
class MinerApplication : public Application {
 public:
  MinerApplication() : Application("xi-miner", "mines blocks to progress the blockchain") {
    useLogging(Logging::ERROR);
    useCurrency();
    useRemoteRpc();
  }

  MinerOptions Options{};

  void makeOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
  int run() override;
};

}  // namespace XiMiner

XI_DECLARE_APP(XiMiner::MinerApplication)

void XiMiner::MinerApplication::makeOptions(cxxopts::Options& options) {
  this->Application::makeOptions(options);
  Options.emplaceOptions(options);
}

bool XiMiner::MinerApplication::evaluateParsedOptions(const cxxopts::Options& options,
                                                      const cxxopts::ParseResult& result) {
  return this->Application::evaluateParsedOptions(options, result) || Options.evaluateParsedOptions(options, result);
}

int XiMiner::MinerApplication::run() {
  auto monitor = UpdateMonitor::start(Options.Address, *currency(), remoteConfiguration(), logger()).takeOrThrow();
  MinerManager miner{remoteConfiguration(), logger()};
  MinerCommandsHandler cli{miner, *monitor, logger()};
  cli.start(true, "xi-miner", Common::Console::Color::Cyan);
  Tools::SignalHandler::install([&miner] { miner.shutdown(); });
  monitor->addObserver(&miner);
  miner.setThreads(Options.Threads);
  cli.minerMonitor().run();
  miner.run();
  cli.minerMonitor().shutdown();
  monitor->removeObserver(&miner);
  cli.requestStop();
  cli.stop();
  monitor->shutdown();
  return 0;
}
