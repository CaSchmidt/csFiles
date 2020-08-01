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

#include <QtCreator/HighlightingItemDelegate.h>

#include "MatchResultsModel.h"

////// MatchResultsRoot - public /////////////////////////////////////////////

MatchResultsRoot::MatchResultsRoot(csAbstractTreeItem *parent)
  : csAbstractTreeItem(parent)
{
}

int MatchResultsRoot::columnCount() const
{
  return 1;
}

QVariant MatchResultsRoot::data(int column, int role) const
{
  if( role == Qt::DisplayRole ) {
    if( column == 0 ) {
      return QStringLiteral("Results");
    }
  }
  return QVariant();
}

////// MatchRestulsFile - public /////////////////////////////////////////////

MatchResultsFile::MatchResultsFile(const QString& filename, MatchResultsRoot *parent)
  : MatchResultsRoot(parent)
  , _filename(filename)
{
}

QVariant MatchResultsFile::data(int column, int role) const
{
  if( role == Qt::DisplayRole ) {
    if( column == 0 ) {
      return _filename;
    }
  }
  return QVariant();
}

////// MatchResultsLine - public /////////////////////////////////////////////

MatchResultsLine::MatchResultsLine(const MatchedLine& line, MatchResultsFile *parent)
  : MatchResultsRoot(parent)
  , _line(line)
{
}

QVariant MatchResultsLine::data(int column, int role) const
{
  using namespace QtCreator;

  if( column == 0 ) {
    if(        role == Qt::DisplayRole ) {
      return _line.text;
    } else if( role == int(HighlightingItemRole::LineNumber) ) {
      return _line.number;
    } else if( role == int(HighlightingItemRole::StartColumn) ) {
      return QVariant::fromValue(_line.start);
    } else if( role == int(HighlightingItemRole::Length) ) {
      return QVariant::fromValue(_line.length);
    } else if( role == int(HighlightingItemRole::Foreground) ) {
      return QColor(Qt::black);
    } else if( role == int(HighlightingItemRole::Background) ) {
      return QColor(Qt::yellow);
    }
  }

  return QVariant();
}
