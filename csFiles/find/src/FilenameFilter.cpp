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

#include <QtCore/QFileInfo>

#include "FilenameFilter.h"

////// public ////////////////////////////////////////////////////////////////

FilenameFilter::~FilenameFilter()
{
}

IFindFilterPtr FilenameFilter::create(const QString& pattern, const bool reject)
{
  return IFindFilterPtr(new FilenameFilter(pattern, reject));
}

////// protected /////////////////////////////////////////////////////////////

bool FilenameFilter::isActive() const
{
  return !_pattern.isEmpty();
}

bool FilenameFilter::isMatch(const QFileInfo& info) const
{
  return _regexp.exactMatch(info.fileName());
}

////// private ///////////////////////////////////////////////////////////////

FilenameFilter::FilenameFilter(const QString& pattern, const bool reject)
  : IFindFilter(reject)
  , _pattern(pattern)
{
  if( _pattern.isEmpty() ) {
    return;
  }
  _regexp.setCaseSensitivity(Qt::CaseInsensitive);
  _regexp.setPatternSyntax(QRegExp::Wildcard);
  _regexp.setPattern(_pattern);
}
