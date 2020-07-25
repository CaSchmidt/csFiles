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

#ifndef FILECACHE_H
#define FILECACHE_H

#include <algorithm>
#include <utility>
#include <vector>

template<typename T>
using CacheView = std::pair<const T*,const T*>;

template<typename T>
inline bool isValid(const CacheView<T>& cv)
{
  return cv.first != nullptr  &&  cv.first <= cv.second;
}

template<typename T>
inline std::size_t diff(const CacheView<T>& cv)
{
  return isValid(cv)
      ? static_cast<std::size_t>(cv.second - cv.first)
      : 0;
}

template<typename T>
class FileCache {
public:
  using buffer_type = std::vector<T>;
  using   size_type = typename buffer_type::size_type;
  using  value_type = typename buffer_type::value_type;

  static_assert(sizeof(value_type) == 1, "Invalid specialization of FileCache<>!");

  // Construction ////////////////////////////////////////////////////////////

  FileCache() = default;
  ~FileCache() = default;

  FileCache(const FileCache&) = default;
  FileCache& operator=(const FileCache&) = default;

  FileCache(FileCache&&) = default;
  FileCache& operator=(FileCache&&) = default;

  // Initialization //////////////////////////////////////////////////////////

  void clear()
  {
    _bot = _top = _cur = 0;
    _buffer.clear();
  }

  bool initialize(const size_type s)
  {
    clear();
    return resize(s);
  }

  // Size ////////////////////////////////////////////////////////////////////

  bool resize(const size_type s)
  {
    if(        s < size() ) {
      return false;
    } else if( s == size() ) {
      return true;
    }

    try {
      _buffer.resize(s);
    } catch(...) {
      clear();
      return false;
    }

    return true;
  }

  inline size_type size() const
  {
    return _buffer.size();
  }

  // Status //////////////////////////////////////////////////////////////////

  inline size_type numFree() const
  {
    return size() - numUsed();
  }

  inline size_type numUsed() const
  {
    return _top - _bot;
  }

  // Buffer Access ///////////////////////////////////////////////////////////

  inline const value_type *first() const
  {
    return numUsed() > 0
        ? _buffer.data() + _cur
        : nullptr;
  }

  inline value_type *free()
  {
    return numFree() > 0
        ? _buffer.data() + numUsed()
        : nullptr;
  }

  inline const value_type *last() const
  {
    return numUsed() > 0
        ? _buffer.data() + numUsed()
        : nullptr;
  }

  inline CacheView<value_type> view() const
  {
    return CacheView<value_type>{first(), last()};
  }

  // Cursor //////////////////////////////////////////////////////////////////

  inline size_type cursor() const
  {
    return _cur;
  }

  size_type moveCursor(const size_type d)
  {
    if( d < 1 ) {
      return _cur;
    }
    _cur = std::min<size_type>(_cur + d, numUsed());
    return _cur;
  }

  // Cache ///////////////////////////////////////////////////////////////////

  bool fill(const size_type s)
  {
    if( s < 1  ||  s > numFree() ) {
      return false;
    }
    _top += s;
    return true;
  }

  void shift()
  {
    // (1) Check cursor's alignment //////////////////////////////////////////

    if( _cur == 0 ) {
      return;
    }

    // (2) Get cursor's view of the cache ////////////////////////////////////

    const CacheView<value_type> cv = view(); // We will move the markers below!

    // (3) Advance cache's view of the file //////////////////////////////////

    _bot += _cur;

    // (4) Align cursor //////////////////////////////////////////////////////

    _cur = 0;

    // (5) Move cached data //////////////////////////////////////////////////

    if( diff(cv) > 0 ) {
      std::copy(cv.first, cv.second, _buffer.data());
    }
  }

  // File Information ////////////////////////////////////////////////////////

  inline size_type bottom() const
  {
    return _bot;
  }

  inline size_type top() const
  {
    return _top;
  }

private:
  buffer_type _buffer{};
  // File Pointers
  size_type _bot{0};
  size_type _top{0};
  // Buffer Offset
  size_type _cur{0};
};

#endif // FILECACHE_H
