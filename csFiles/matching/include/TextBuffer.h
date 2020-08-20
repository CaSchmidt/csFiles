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

#ifndef TEXTBUFFER_H
#define TEXTBUFFER_H

#include <cstddef>

#include <memory>
#include <vector>

#include "TextInfo.h"

class QIODevice;

using TextBufferPtr = std::unique_ptr<class TextBuffer>;

class TextBuffer {
public:
  using size_type = TextFileCache::size_type;

  TextBuffer(TextBuffer&&) noexcept = default;
  TextBuffer& operator=(TextBuffer&&) noexcept = default;

  ~TextBuffer() noexcept;

  const TextInfo& info() const;
  bool isValid() const;

  bool hasNextLine() const;
  TextLine nextLine(const bool keepEnding = true, bool *ok = nullptr);

  // NOTE: TextBuffer takes ownership of 'device'!
  static TextBufferPtr create(QIODevice *device);

private:
  TextBuffer() noexcept = delete;

  TextBuffer(const TextBuffer&) noexcept = delete;
  TextBuffer& operator=(const TextBuffer&) noexcept = delete;

  TextBuffer(QIODevice *device) noexcept;

  bool canFill() const;
  bool canGrow() const;
  bool cursorAtEof() const;
  bool eofCached() const;
  bool fillCache();
  const char *findNextLine() const;
  bool growCache();

  TextFileCache _cache{};
  QIODevice    *_device{nullptr};
  TextInfo      _info{};
};

#endif // TEXTBUFFER_H
