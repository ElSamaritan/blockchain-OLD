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

#pragma once

#include <map>
#include <typeindex>
#include <typeinfo>
#include <deque>
#include <utility>
#include <vector>
#include <chrono>
#include <memory>

namespace CryptoNote {
class P2pConnectionContextHistory {
 public:
  using clock_t = std::chrono::system_clock;
  using time_point = clock_t::time_point;

  struct IObservedOccurrence {
    virtual ~IObservedOccurrence() = default;

    time_point timestamp;

    IObservedOccurrence(time_point t) : timestamp{t} {}
  };

  template <typename _ObservedT>
  struct ObservedOccurrence : IObservedOccurrence {
    static inline const std::type_info& type() { return typeid(_ObservedT); }

    _ObservedT value;

    ObservedOccurrence(time_point t, _ObservedT o) : IObservedOccurrence(t), value{std::move(o)} {}
  };

 private:
  std::map<std::type_index, std::deque<std::shared_ptr<IObservedOccurrence>>> m_timeline{};

 public:
  template <typename _ObservedT>
  inline std::vector<std::shared_ptr<const ObservedOccurrence<_ObservedT>>> getTimeline() const {
    auto search = this->m_timeline.find(typeid(_ObservedT));
    if (search == this->m_timeline.end()) {
      return {};
    } else {
      std::vector<std::shared_ptr<const ObservedOccurrence<_ObservedT>>> reval;
      reval.reserve(search->second.size());
      for (const auto& iOcc : search->second) {
        reval.emplace_back(std::static_pointer_cast<const ObservedOccurrence<_ObservedT>>(iOcc));
      }
      return reval;
    }
  }

  template <typename _ObservedT>
  inline void pushOccurrence(_ObservedT occ, size_t maxSize) {
    auto& timeline = this->m_timeline[typeid(_ObservedT)];
    timeline.emplace_back(std::make_shared<ObservedOccurrence<_ObservedT>>(clock_t::now(), std::move(occ)));
    while (timeline.size() > maxSize) {
      timeline.pop_front();
    }
  }

  inline void clear() { this->m_timeline.clear(); }
};
}  // namespace CryptoNote
