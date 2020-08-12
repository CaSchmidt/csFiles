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

constexpr PCRE2_SIZE kErrorLength = 1024;

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  PCRE2_SIZE skipUtf8(const char *str, const PCRE2_SIZE length, PCRE2_SIZE offset)
  {
    while( offset < length  &&  (str[offset] & 0xC0) == 0x80 ) {
      offset++;
    }
    return offset;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

Pcre2Matcher::~Pcre2Matcher()
{
  clear(true);
}

IMatcherPtr Pcre2Matcher::clone() const
{
  IMatcherPtr result{new Pcre2Matcher(this)};
  if( result  &&  !result->isCompiled() ) {
    result.reset();
  }
  return result;
}

bool Pcre2Matcher::compile(const std::string& pattern)
{
  clear();
  if( !hasCompileContext() ) {
    return false;
  }
  if( pattern.empty() ) {
    return false;
  }
  _regexp = pcre2_compile_8(reinterpret_cast<PCRE2_SPTR8>(pattern.data()), pattern.size(),
                            compileOptions(), &_errcode, &_erroffset, _ccontext);
  if( _regexp == nullptr ) {
    return isCompiled();
  } else {
    resetError(); // NOTE: pcre2_compile() returns COMPILE_ERROR_BASE == 100 upon success!
  }
  initMatchData();
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
  return hasCompileContext()  &&  _regexp != nullptr  &&  _mdata != nullptr  &&  _ovector != nullptr;
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
  bool ok = false;
  if(        eol == EndOfLine::Cr ) {
    ok = pcre2_set_newline_8(_ccontext, PCRE2_NEWLINE_CR) == 0;
  } else if( eol == EndOfLine::CrLf ) {
    ok = pcre2_set_newline_8(_ccontext, PCRE2_NEWLINE_CRLF) == 0;
  } else if( eol == EndOfLine::Lf ) {
    ok = pcre2_set_newline_8(_ccontext, PCRE2_NEWLINE_LF) == 0;
  }
#if 0
  if( ok  &&  isCompiled() ) {
    recompile();
  }
#endif
  return ok;
}

////// static public /////////////////////////////////////////////////////////

IMatcherPtr Pcre2Matcher::create()
{
  IMatcherPtr result{new Pcre2Matcher()};
  if( result  &&  !dynamic_cast<Pcre2Matcher*>(result.get())->hasCompileContext() ) {
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

  const PCRE2_SIZE length = first != nullptr  &&  first < last
      ? static_cast<PCRE2_SIZE>(last - first)
      : 0;
  if( length < 1 ) {
    return false;
  }

  const int rc = pcre2_match_8(_regexp, reinterpret_cast<PCRE2_SPTR8>(first), length, 0,
                               matchOptions(), _mdata, nullptr);
  if(        rc < 0 ) {
    _errcode = rc;
    return false;
  } else if( rc > 0 ) {
    storeMatch();
  }

  if( flags().testFlag(MatchFlag::FindAll) ) {
    return nextMatches(first, length);
  }

  return hasMatch();
}

////// private ///////////////////////////////////////////////////////////////

Pcre2Matcher::Pcre2Matcher()
  : IMatcher()
{
  _ccontext = pcre2_compile_context_create_8(nullptr);
}

Pcre2Matcher::Pcre2Matcher(const Pcre2Matcher *other)
  : IMatcher(*other)
{
  _ccontext = pcre2_compile_context_copy_8(other->_ccontext);
  _regexp   = pcre2_code_copy_with_tables_8(other->_regexp);
  initMatchData();
}

void Pcre2Matcher::clear(const bool all)
{
  resetError();
  resetMatch();
  resetPattern();

  _ovector = nullptr;
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
  if( flags().testFlag(MatchFlag::CaseInsensitive) ) {
    options |= PCRE2_CASELESS;
  }
  if( !flags().testFlag(MatchFlag::RegExp) ) {
    options |= PCRE2_LITERAL;
  }
  if( flags().testFlag(MatchFlag::Utf8) ) {
    options |= PCRE2_UTF | PCRE2_UCP;
  }
  return options;
}

bool Pcre2Matcher::hasCompileContext() const
{
  return _ccontext != nullptr;
}

bool Pcre2Matcher::initMatchData()
{
  if( _regexp == nullptr ) {
    return false;
  }
  _mdata = pcre2_match_data_create_from_pattern_8(_regexp, nullptr);
  if( _mdata != nullptr ) {
    _ovector = pcre2_get_ovector_pointer_8(_mdata);
  }
  return _mdata != nullptr  &&  _ovector != nullptr;
}

bool Pcre2Matcher::isNewlineCrLf() const
{
  if( _regexp == nullptr ) {
    return false;
  }
  uint32_t newline = 0;
  if( pcre2_pattern_info_8(_regexp, PCRE2_INFO_NEWLINE, &newline) != 0 ) {
    return false;
  }
  return
      newline == PCRE2_NEWLINE_ANY      ||
      newline == PCRE2_NEWLINE_ANYCRLF  ||
      newline == PCRE2_NEWLINE_CRLF;
}

bool Pcre2Matcher::isUtf8() const
{
  if( _regexp == nullptr ) {
    return false;
  }
  uint32_t options = 0;
  if( pcre2_pattern_info_8(_regexp, PCRE2_INFO_ALLOPTIONS, &options) != 0 ) {
    return false;
  }
  return (options & PCRE2_UTF) != 0;
}

bool Pcre2Matcher::isValidMatch() const
{
  return _ovector != nullptr  &&  _ovector[0] <= _ovector[1];
}

uint32_t Pcre2Matcher::matchOptions() const
{
  return 0;
}

bool Pcre2Matcher::nextMatches(const char *first, const PCRE2_SIZE length)
{
  const bool      is_crlf = isNewlineCrLf();
  const bool      is_utf8 = isUtf8();
  const uint32_t options0 = matchOptions();
  while( true ) {
    resetError();

    uint32_t  options = options0;
    PCRE2_SIZE offset = _ovector[1];

    if( _ovector[0] == _ovector[1] ) {
      if( _ovector[0] == length ) {
        break;
      }
      options |= PCRE2_ANCHORED | PCRE2_NOTEMPTY_ATSTART;

    } else {
      const PCRE2_SIZE start = pcre2_get_startchar_8(_mdata);
      if( offset <= start ) {
        if( start >= length ) {
          break;
        }
        offset = start + 1;
        if( is_utf8 ) {
          offset = priv::skipUtf8(first, length, offset);
        }
      }

    }

    const int rc = pcre2_match_8(_regexp, reinterpret_cast<PCRE2_SPTR8>(first), length, offset,
                                 options, _mdata, nullptr);

    if( rc == PCRE2_ERROR_NOMATCH ) {
      if( options == options0 ) {
        break;
      }
      _ovector[1] = offset + 1;
      if( is_crlf  &&  offset < length - 1  &&
          first[offset] == '\r'  &&  first[offset + 1] == '\n' ) {
        _ovector[1] += 1;
      } else if( is_utf8 ) {
        _ovector[1] = priv::skipUtf8(first, length, _ovector[1]);
      }
      continue;
    }

    if(        rc < 0 ) {
      _errcode = rc;
      return false;
    } else if( rc > 0 ) {
      storeMatch();
    }
  } // while()

  return hasMatch();
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

bool Pcre2Matcher::storeMatch()
{
  if( !isValidMatch() ) {
    return false;
  }
  const int  start = static_cast<int>(_ovector[0]);
  const int length = static_cast<int>(_ovector[1] - _ovector[0]);
  _match.emplace_back(start, length);
  return true;
}
