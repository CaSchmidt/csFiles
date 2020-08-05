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

////// MatchResultsItem - public /////////////////////////////////////////////

MatchResultsItem::MatchResultsItem(csAbstractTreeItem *parent)
  : csAbstractTreeItem(parent)
{
}

int MatchResultsItem::columnCount() const
{
  return 1;
}

////// MatchResultsRoot - public /////////////////////////////////////////////

MatchResultsRoot::MatchResultsRoot(const QString& rootPath)
  : MatchResultsItem(nullptr)
  , _rootPath(rootPath)
{
  if( !_rootPath.isEmpty() ) {
    _root.setPath(_rootPath);
  }
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

QString MatchResultsRoot::displayFilename(const QString& filename) const
{
  return !_rootPath.isEmpty()  &&  filename.startsWith(_rootPath)
      ? _root.relativeFilePath(filename)
      : filename;
}

QStringList MatchResultsRoot::files() const
{
  QStringList result;
  for(int i = 0; i < rowCount(); i++) {
    csAbstractTreeItem *item = childItem(i);
    MatchResultsFile   *file = dynamic_cast<MatchResultsFile*>(item);
    if( file == nullptr ) {
      continue;
    }
    result.push_back(file->filename());
  }
  return result;
}

QString MatchResultsRoot::rootPath() const
{
  return _rootPath;
}

////// MatchRestulsFile - public /////////////////////////////////////////////

MatchResultsFile::MatchResultsFile(const QString& filename, MatchResultsRoot *parent)
  : MatchResultsItem(parent)
  , _filename(filename)
{
}

QVariant MatchResultsFile::data(int column, int role) const
{
  if( column == 0 ) {
    if(        role == Qt::DisplayRole ) {
      return dynamic_cast<const MatchResultsRoot*>(parentItem())->displayFilename(_filename);
    } else if( role == Qt::ToolTipRole ) {
      return _filename;
    }
  }
  return QVariant();
}

QString MatchResultsFile::filename() const
{
  return _filename;
}

////// MatchResultsLine - public /////////////////////////////////////////////

MatchResultsLine::MatchResultsLine(const MatchedLine& line, MatchResultsFile *parent)
  : MatchResultsItem(parent)
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

int MatchResultsLine::number() const
{
  return _line.number;
}
