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

#include <QtCore/QIODevice>

#include "TextBuffer.h"

////// Constants /////////////////////////////////////////////////////////////

#ifdef HAVE_TEXTBUFFER_UNITTEST
constexpr TextBuffer::size_type kIniBufferSize =  8;
constexpr TextBuffer::size_type kMaxBufferSize = 32;
#else
constexpr TextBuffer::size_type kIniBufferSize =  128*1024;
constexpr TextBuffer::size_type kMaxBufferSize = 1024*1024;
#endif
constexpr TextBuffer::size_type kTextInfoSize  =      1024;

////// public ////////////////////////////////////////////////////////////////

TextBuffer::~TextBuffer() noexcept
{
  delete _device;
}

const TextInfo& TextBuffer::info() const
{
  return _info;
}

bool TextBuffer::isValid() const
{
  return _cache.size() > 0  &&  _device != nullptr;
}

bool TextBuffer::hasNextLine() const
{
  return _info.isValid()  &&  !cursorAtEof();
}

TextLine TextBuffer::nextLine(const bool keepEnding, bool *ok)
{
  if( ok != nullptr ) {
    *ok = false;
  }

  if( !hasNextLine() ) {
    return TextLine();
  }

  TextLine line;
  while( true ) {
    line.first  = _cache.first(); // fillCache() & growCache() may move the cursor!
    line.second = findNextLine();
    if( line.second != nullptr ) { // (1) Ending found in cache!
      break;
    }

    if( eofCached() ) { // (2) Line without ending is the last one!
      line.second = _cache.last();
      break;
    }

    _cache.shift();
    if( canFill() ) {
      if( !fillCache() ) { // Option 1: Try to fill the cache.
        return TextLine();
      }
    } else {
      if( !growCache()  ||  !fillCache() ) { // Option 2: Try to grow & fill the cache.
        return TextLine();
      }
    }
  }

  _cache.moveCursor(diff(line));

  if( !keepEnding ) {
    line = _info.removeEnding(line);
  }

  if( ok != nullptr ) {
    *ok = true;
  }

  return line;
}

TextBufferPtr TextBuffer::create(QIODevice *device)
{
  TextBufferPtr result(new TextBuffer(device));
  if( !result->isValid() ) {
    delete device;
    result.reset();
  }
  return result;
}

////// private ///////////////////////////////////////////////////////////////

TextBuffer::TextBuffer(QIODevice *device) noexcept
  : _device{device}
{
  _cache.initialize(kIniBufferSize);
  if( !isValid()  ||  !fillCache() ) {
    return;
  }
  const TextLine l = _cache.view();
  const size_type scanLen = std::min<size_type>(diff(l), kTextInfoSize);
  _info = TextInfo::scan(l.first, l.first + scanLen);
}

bool TextBuffer::canFill() const
{
  return _cache.numFree() > 0;
}

bool TextBuffer::canGrow() const
{
  return _cache.size() < kMaxBufferSize;
}

bool TextBuffer::cursorAtEof() const
{
  return eofCached()  &&  _cache.cursor() == _cache.numUsed();
}

bool TextBuffer::eofCached() const
{
  return _cache.top() == static_cast<size_type>(_device->size());
}

bool TextBuffer::fillCache()
{
  if( eofCached() ) {
    return true;
  }
  if( !canFill() ) {
    return false;
  }
  const size_type got = _device->read(_cache.free(), _cache.numFree());
  return _cache.fill(got);
}

const char *TextBuffer::findNextLine() const
{
  if(        _info.eolType() == EndOfLine::Cr ) {
    for(const char *ptr = _cache.first(); ptr < _cache.last(); ++ptr) {
      if( *ptr == '\r' ) {
        return ptr + 1;
      }
    }
  } else if( _info.eolType() == EndOfLine::CrLf ) {
    for(const char *ptr = _cache.first(); ptr < _cache.last(); ++ptr) {
      if( *ptr == '\r'  &&  ptr + 1 < _cache.last()  &&  *++ptr == '\n' ) {
        return ptr + 1;
      }
    }
  } else if( _info.eolType() == EndOfLine::Lf ) {
    for(const char *ptr = _cache.first(); ptr < _cache.last(); ++ptr) {
      if( *ptr == '\n' ) {
        return ptr + 1;
      }
    }
  }
  return nullptr;
}

bool TextBuffer::growCache()
{
  if( !canGrow() ) {
    return false;
  }
  const size_type s = std::min<size_type>(_cache.size()*2, kMaxBufferSize);
  return _cache.resize(s);
}
