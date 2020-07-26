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

#include <algorithm>
#include <iterator>

#include "FilesModel.h"

////// public ////////////////////////////////////////////////////////////////

FilesModel::FilesModel(QObject *parent)
  : QAbstractListModel(parent)
{
}

FilesModel::~FilesModel()
{
}

QVariant FilesModel::data(const QModelIndex& index, int role) const
{
  if( !index.isValid() ) {
    return QVariant();
  }
  if(        role == Qt::DisplayRole ) {
    return !_rootPath.isEmpty()  &&  _files[index.row()].startsWith(_rootPath)
        ? _root.relativeFilePath(_files[index.row()])
        : _files[index.row()];
    } else if( role == Qt::DecorationRole ) {
    return _iconProvider.icon(QFileInfo(_files[index.row()]));
  } else if( role == Qt::EditRole ) {
    return _files[index.row()];
  } else if( role == Qt::ToolTipRole ) {
    return _files[index.row()];
  }
  return QVariant();
}

Qt::ItemFlags FilesModel::flags(const QModelIndex& index) const
{
  return QAbstractListModel::flags(index);
}

QVariant FilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(section);
  Q_UNUSED(orientation);
  Q_UNUSED(role);
  return QVariant();
}

int FilesModel::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return _files.size();
}

void FilesModel::append(const QStringList& files)
{
  if( files.size() < 1 ) {
    return;
  }
  beginInsertRows(QModelIndex(), rowCount(), rowCount() + files.size() - 1);
  _files.reserve(rowCount() + files.size());
  std::copy(files.cbegin(), files.cend(), std::back_inserter(_files));
  endInsertRows();
}

void FilesModel::clear()
{
  beginResetModel();
  _files.clear();
  _root = QDir();
  _rootPath.clear();
  endResetModel();
}

void FilesModel::clearRoot()
{
  _rootPath.clear();
  const QModelIndex from = index(0);
  const QModelIndex   to = index(rowCount() - 1);
  emit dataChanged(from, to);
}

QStringList FilesModel::files() const
{
  return _files.toList();
}

void FilesModel::remove(const QStringList& files)
{
  beginResetModel();
  for(const QString& file : files) {
    _files.removeAll(file);
  }
  if( _files.isEmpty() ) {
    _rootPath.clear();
  }
  endResetModel();
}

bool FilesModel::setRoot(const QDir& root)
{
  if( !_rootPath.isEmpty() ) {
    return false;
  }
  _root = root;
  _rootPath = _root.absolutePath();
  const QModelIndex from = index(0);
  const QModelIndex   to = index(rowCount() - 1);
  emit dataChanged(from, to);
  return true;
}