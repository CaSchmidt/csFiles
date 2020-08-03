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

#ifndef PCRE2MATCHER_H
#define PCRE2MATCHER_H

#include <pcre2.h>

#include "IMatcher.h"

class Pcre2Matcher : public IMatcher {
public:
  ~Pcre2Matcher();

  IMatcherPtr clone() const;
  bool compile(const std::string& pattern);
  std::string error() const;
  MatchList getMatch() const;
  bool hasMatch() const;
  bool isCompiled() const;
  bool isError() const;
  bool setEndOfLine(const EndOfLine eol);

  static IMatcherPtr create();

protected:
  bool impl_match(const char *first, const char *last);

private:
  Pcre2Matcher();
  Pcre2Matcher(const Pcre2Matcher *other);

  Pcre2Matcher(const Pcre2Matcher&) = delete;
  Pcre2Matcher& operator=(const Pcre2Matcher&) = delete;

  Pcre2Matcher(Pcre2Matcher&&) = delete;
  Pcre2Matcher& operator=(Pcre2Matcher&&) = delete;

  void clear(const bool all = false);
  uint32_t compileOptions() const;
  bool hasCompileContext() const;
  bool initMatchData();
  bool isNewlineCrLf() const;
  bool isUtf8() const;
  bool isValidMatch() const;
  uint32_t matchOptions() const;
  bool nextMatches(const char *first, const PCRE2_SIZE length);
  void resetError();
  void resetMatch();
  bool storeMatch();

  pcre2_compile_context_8 *_ccontext{nullptr};
  int _errcode{0};
  PCRE2_SIZE _erroffset{PCRE2_SIZE_MAX};
  MatchList _match{};
  pcre2_match_data_8 *_mdata{nullptr};
  pcre2_code_8 *_regexp{nullptr};
  PCRE2_SIZE *_ovector{nullptr};
};

#endif // PCRE2MATCHER_H
