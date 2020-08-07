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

#include "ExtensionFilter.h"

#include "PatternList.h"

////// public ////////////////////////////////////////////////////////////////

ExtensionFilter::~ExtensionFilter()
{
}

IFindFilterPtr ExtensionFilter::create(const QString& extensions, const bool reject,
                                       const bool complete)
{
  return IFindFilterPtr(new ExtensionFilter(extensions, reject, complete));
}

////// protected /////////////////////////////////////////////////////////////

bool ExtensionFilter::isActive() const
{
  return !_extensions.isEmpty();
}

bool ExtensionFilter::isMatch(const QFileInfo& info) const
{
  if( _complete ) {
    return _extensions.contains(info.completeSuffix(), Qt::CaseInsensitive);
  }
  return _extensions.contains(info.suffix(), Qt::CaseInsensitive);
}

////// private ///////////////////////////////////////////////////////////////

ExtensionFilter::ExtensionFilter(const QString& extensions, const bool reject,
                                 const bool complete)
  : IFindFilter(reject)
  , _complete{complete}
{
  _extensions = preparePatternList(extensions);
}
