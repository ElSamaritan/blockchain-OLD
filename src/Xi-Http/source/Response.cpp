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

#include "Xi/Http/Response.h"

#include <cinttypes>

Xi::Http::Response::Response(StatusCode code, const std::string &body, ContentType type)
    : m_headers{}, m_body{body}, m_statusCode{code} {
  headers().setContentType(type);
}

Xi::Http::StatusCode Xi::Http::Response::status() const { return m_statusCode; }

void Xi::Http::Response::setStatus(Xi::Http::StatusCode code) { m_statusCode = code; }

const std::string &Xi::Http::Response::body() const { return m_body; }

void Xi::Http::Response::setBody(const std::string &_body) {
  std::string movableBody{_body};
  setBody(std::move(movableBody));
}

void Xi::Http::Response::setBody(std::string &&_body) { m_body = std::move(_body); }

bool Xi::Http::Response::isRedirection() const { return (static_cast<uint32_t>(status()) / 100) == 3; }

Xi::Http::HeaderContainer &Xi::Http::Response::headers() { return m_headers; }

const Xi::Http::HeaderContainer &Xi::Http::Response::headers() const { return m_headers; }
