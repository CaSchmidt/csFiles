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

#include "Pcre2Matcher.h"

////// Constants /////////////////////////////////////////////////////////////

constexpr std::size_t kErrorLength = 1024;

////// public ////////////////////////////////////////////////////////////////

Pcre2Matcher::~Pcre2Matcher()
{
  clear(true);
}

bool Pcre2Matcher::compile(const std::string& pattern)
{
  clear();
  if( !hasCompileContext() ) {
    return false;
  }
  _regexp = pcre2_compile_8(reinterpret_cast<PCRE2_SPTR8>(pattern.data()), pattern.size(),
                            compileOptions(), &_errcode, &_erroffset, _ccontext);
  if( _regexp == nullptr ) {
    return isCompiled();
  } else {
    resetError(); // NOTE: pcre2_compile() returns COMPILE_ERROR_BASE == 100 upon success!
  }
  _mdata = pcre2_match_data_create_from_pattern_8(_regexp, nullptr);
  if( isCompiled() ) {
    setPattern(pattern);
  }
  return isCompiled();
}

std::string Pcre2Matcher::error() const
{
  if( _errcode == 0 ) {
    return std::string();
  }
  std::string str;
  try {
    str.resize(kErrorLength, cs::glyph<std::string::value_type>::null);
    pcre2_get_error_message_8(_errcode, reinterpret_cast<PCRE2_UCHAR8*>(str.data()), str.size());
    cs::shrink(str);
  } catch(...) {
    str.clear();
  }
  return str;
}

MatchList Pcre2Matcher::getMatch() const
{
  return _match;
}

bool Pcre2Matcher::hasMatch() const
{
  return !_match.empty();
}

bool Pcre2Matcher::isCompiled() const
{
  return hasCompileContext()  &&  _regexp != nullptr  &&  _mdata != nullptr;
}

bool Pcre2Matcher::isError() const
{
  return _errcode != 0;
}

bool Pcre2Matcher::setEndOfLine(const EndOfLine eol)
{
  if( !hasCompileContext() ) {
    return false;
  }
  if(        eol == EndOfLine::Cr ) {
    return pcre2_set_newline_8(_ccontext, PCRE2_NEWLINE_CR) == 0;
  } else if( eol == EndOfLine::CrLf ) {
    return pcre2_set_newline_8(_ccontext, PCRE2_NEWLINE_CRLF) == 0;
  } else if( eol == EndOfLine::Lf ) {
    return pcre2_set_newline_8(_ccontext, PCRE2_NEWLINE_LF) == 0;
  }
  return false;
}

////// static public /////////////////////////////////////////////////////////

IMatcherPtr Pcre2Matcher::create()
{
  IMatcherPtr result{new Pcre2Matcher()};
  if( !dynamic_cast<Pcre2Matcher*>(result.get())->hasCompileContext() ) {
    result.reset();
  }
  return result;
}

////// protected /////////////////////////////////////////////////////////////

bool Pcre2Matcher::impl_match(const char *first, const char *last)
{
  resetError();
  resetMatch();

  if( !isCompiled() ) {
    return false;
  }

  const std::size_t len = first != nullptr  &&  first < last
      ? static_cast<std::size_t>(last - first)
      : 0;
  if( len < 1 ) {
    return false;
  }

  const int rc = pcre2_match_8(_regexp, reinterpret_cast<PCRE2_SPTR8>(first), len, 0,
                               matchOptions(), _mdata, nullptr);
  if(        rc < 0 ) {
    _errcode = rc;
  } else if( rc > 0 ) {
    std::size_t *output = pcre2_get_ovector_pointer_8(_mdata);
    if( output[0] <= output[1] ) {
      const int  start = static_cast<int>(output[0]);
      const int length = static_cast<int>(output[1] - output[0]);
      _match.emplace_back(start, length);
    }
  }

  return rc > 0;
}

////// private ///////////////////////////////////////////////////////////////

Pcre2Matcher::Pcre2Matcher()
{
  _ccontext = pcre2_compile_context_create_8(nullptr);
}

void Pcre2Matcher::clear(const bool all)
{
  resetError();
  resetMatch();
  resetPattern();

  if( _mdata != nullptr ) {
    pcre2_match_data_free_8(_mdata);
    _mdata = nullptr;
  }
  if( _regexp != nullptr ) {
    pcre2_code_free_8(_regexp);
    _regexp = nullptr;
  }

  if( !all ) {
    return;
  }
  if( _ccontext != nullptr ) {
    pcre2_compile_context_free_8(_ccontext);
    _ccontext = nullptr;
  }
}

uint32_t Pcre2Matcher::compileOptions() const
{
  uint32_t options = 0;
  if( ignoreCase() ) {
    options |= PCRE2_CASELESS;
  }
  if( !matchRegExp() ) {
    options |= PCRE2_LITERAL;
  }
  return options;
}

bool Pcre2Matcher::hasCompileContext() const
{
  return _ccontext != nullptr;
}

uint32_t Pcre2Matcher::matchOptions() const
{
  return 0;
}

void Pcre2Matcher::resetError()
{
  _errcode = 0;
  _erroffset = PCRE2_SIZE_MAX;
}

void Pcre2Matcher::resetMatch()
{
  _match.clear();
}