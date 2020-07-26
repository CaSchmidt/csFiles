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

#include "TextInfo.h"

////// public ////////////////////////////////////////////////////////////////

EndOfLine TextInfo::eolType() const
{
  return _eol;
}

bool TextInfo::isBinary() const
{
  return _binary;
}

bool TextInfo::isValid() const
{
  return _eol != EndOfLine::Unknown  &&  !isBinary();
}

TextLine TextInfo::removeEnding(const TextLine& line) const
{
  const std::size_t len = diff(line);
  if(        _eol == EndOfLine::Cr    &&  len >= 1  &&
             line.first[len - 1] == '\r' ) {
    return TextLine{line.first, line.second - 1};
  } else if( _eol == EndOfLine::CrLf  &&  len >= 2  &&
             line.first[len - 2] == '\r'  &&  line.first[len - 1] == '\n' ) {
    return TextLine{line.first, line.second - 2};
  } else if( _eol == EndOfLine::Lf    &&  len >= 1  &&
             line.first[len - 1] == '\n') {
    return TextLine{line.first, line.second - 1};
  }
  return line;
}

TextInfo TextInfo::scan(const char *first, const char *last)
{
  TextInfo result;

  int cntCr   = 0;
  int cntCrLf = 0;
  int cntLf   = 0;

  for(const char *ptr = first; ptr < last; ++ptr) {
    if(        *ptr == '\0' ) {
      result._binary = true;
    } else if( *ptr == '\n' ) {
      cntLf += 1;
    } else if( *ptr == '\r' ) {
      if( ptr + 1 < last  &&  *++ptr == '\n' ) {
        cntCrLf += 1;
      } else {
        cntCr += 1;
      }
    }
  }

  if( cntCr > 0 ) {
    result._eol = EndOfLine::Cr;
  }
  if( cntCrLf > cntCr ) {
    result._eol = EndOfLine::CrLf;
  }
  if( cntLf > cntCrLf ) {
    result._eol = EndOfLine::Lf;
  }

  return result;
}
