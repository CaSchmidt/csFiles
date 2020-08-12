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

#ifndef IMATCHER_H
#define IMATCHER_H

#include <cstdint>

#include <list>
#include <memory>
#include <string>
#include <utility>

#include <csUtil/csFlags.h>

#include "TextInfo.h"

enum class MatchFlag : unsigned {
  NoFlags         = 0,
  CaseInsensitive = 1,
  FindAll         = 2,
  RegExp          = 4,
  Utf8            = 8
};

CS_ENABLE_FLAGS(MatchFlag);

using MatchFlags = csFlags<MatchFlag>;

using Match     = std::pair<int,int>;
using MatchList = std::list<Match>;

using IMatcherPtr = std::unique_ptr<class IMatcher>;

class IMatcher {
public:
  IMatcher();
  IMatcher(const IMatcher&);
  virtual ~IMatcher();

  virtual IMatcherPtr clone() const = 0;
  virtual bool compile(const std::string& pattern) = 0;
  virtual std::string error() const = 0;
  virtual MatchList getMatch() const = 0;
  virtual bool hasMatch() const = 0;
  virtual bool isCompiled() const = 0;
  virtual bool isError() const = 0;
  virtual bool setEndOfLine(const EndOfLine eol) = 0;

  MatchFlags flags() const;
  void setFlags(const MatchFlags f);

  bool match(const char *str);
  bool match(const char *str, const std::size_t len);
  bool match(const std::string& str);
  bool match(const char *first, const char *last);

  bool recompile();

  std::string pattern() const;

protected:
  virtual bool impl_match(const char *first, const char *last) = 0;
  void resetPattern();
  void setPattern(const std::string& pattern);

private:
  IMatcher& operator=(const IMatcher&) = delete;

  IMatcher(IMatcher&&) = delete;
  IMatcher& operator=(IMatcher&&) = delete;

  MatchFlags _flags{MatchFlag::NoFlags};
  std::string _pattern{};
};

IMatcherPtr createDefaultMatcher();

IMatcherPtr createPcre2Matcher();

#endif // IMATCHER_H
