/****************************************************************************
** Copyright (c) 2020, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include <csUtil/csStringUtil.h>

#include "IMatcher.h"

////// public ////////////////////////////////////////////////////////////////

IMatcher::IMatcher() = default;

IMatcher::IMatcher(const IMatcher&) = default;

IMatcher::~IMatcher()
{
}

MatchFlags IMatcher::flags() const
{
  return _flags;
}

void IMatcher::setFlags(const MatchFlags f)
{
  _flags = f;
}

bool IMatcher::match(const char *str)
{
  return impl_match(str, str + cs::length(str));
}

bool IMatcher::match(const char *str, const std::size_t len)
{
  return impl_match(str, str + len);
}

bool IMatcher::match(const std::string& str)
{
  return impl_match(str.data(), str.data() + str.size());
}

bool IMatcher::match(const char *first, const char *last)
{
  return impl_match(first, last);
}

bool IMatcher::recompile()
{
  return compile(pattern());
}

std::string IMatcher::pattern() const
{
  return _pattern;
}

////// protected /////////////////////////////////////////////////////////////

void IMatcher::resetPattern()
{
  _pattern.clear();
}

void IMatcher::setPattern(const std::string& pattern)
{
  _pattern = pattern;
}
