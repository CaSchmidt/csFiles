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

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>

#include <csQt/csQtUtil.h>

#include "WFileList.h"

#include "FilesModel.h"

////// public ////////////////////////////////////////////////////////////////

WFileList::WFileList(QWidget *parent, Qt::WindowFlags f)
  : csWListEditor(parent, f)
{
  // User Interface //////////////////////////////////////////////////////////

  button(Up)->setVisible(false);
  button(Down)->setVisible(false);

  setShowContextMenu(true);

  view()->setAlternatingRowColors(true);
  view()->setEditTriggers(QAbstractItemView::NoEditTriggers);
  view()->setSelectionBehavior(QAbstractItemView::SelectRows);
  view()->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // Model ///////////////////////////////////////////////////////////////////

  _model = new FilesModel(this);
  view()->setModel(_model);
}

WFileList::~WFileList()
{
}

bool WFileList::autoCurrentDir() const
{
  return _autoCurrentDir;
}

void WFileList::setAutoCurrentDir(const bool on)
{
  _autoCurrentDir = on;
  emit autoCurrentDirChanged(autoCurrentDir());
}

bool WFileList::autoRoot() const
{
  return _autoRoot;
}

void WFileList::setAutoRoot(const bool on)
{
  _autoRoot = on;
  if( !autoRoot() ) {
    clearRoot();
  }
  emit autoRootChanged(autoRoot());
}

bool WFileList::listFilesOnly() const
{
  return _model->listFilesOnly();
}

void WFileList::setListFilesOnly(const bool on)
{
  _model->setListFilesOnly(on);
  emit listFilesOnlyChanged(listFilesOnly());
}

QString WFileList::selectionFilter() const
{
  return _selectionFilter;
}

void WFileList::setSelectionFilter(const QString& filter)
{
  _selectionFilter = filter;
  emit selectionFilterChanged(selectionFilter());
}

int WFileList::count() const
{
  return _model->rowCount();
}

QStringList WFileList::files() const
{
  return _model->files();
}

QString WFileList::rootPath() const
{
  return _model->rootPath();
}

////// public slots //////////////////////////////////////////////////////////

void WFileList::appendFiles(const QStringList& files)
{
  _model->append(files);
}

void WFileList::appendFiles(const QDir& root, const QStringList& files)
{
  appendFiles(files);
  if( autoRoot() ) {
    _model->setRoot(root);
  }
}

void WFileList::appendFiles(const QString& rootPath, const QStringList& files)
{
  appendFiles(QDir(rootPath), files);
}

void WFileList::clearList()
{
  _model->clear();
}

void WFileList::clearRoot()
{
  _model->clearRoot();
}

void WFileList::copyList()
{
  QStringList files = WFileList::files();
  if( files.isEmpty() ) {
    return;
  }
  qSort(files);
  csSetClipboardText(files);
}

////// private ///////////////////////////////////////////////////////////////

void WFileList::onAdd()
{
  const QStringList files =
      QFileDialog::getOpenFileNames(this, tr("Select files"), QDir::currentPath(),
                                    selectionFilter().isEmpty()
                                    ? tr("All files (*.*)")
                                    : selectionFilter());
  if( files.isEmpty() ) {
    return;
  }
  const QString path = QFileInfo(files.front()).absolutePath();
  appendFiles(path, files);
  if( autoCurrentDir() ) {
    QDir::setCurrent(path);
  }
}

void WFileList::onContextMenu(const QPoint& globalPos)
{
  QMenu menu;
  QAction *copyList = menu.addAction(tr("Copy list"));
  menu.addSeparator();
  QAction *clearList = menu.addAction(tr("Clear list"));
  menu.addSeparator();
  QAction *clearRoot = menu.addAction(tr("Clear root"));

  QAction *choice = menu.exec(globalPos);
  if(        choice == nullptr ) {
    return;
  } else if( choice == copyList ) {
    WFileList::copyList();
  } else if( choice == clearRoot ) {
    WFileList::clearRoot();
  } else if( choice == clearList ) {
    WFileList::clearList();
  }
}

void WFileList::onRemove()
{
  const QModelIndexList selection = view()->selectionModel()->selectedIndexes();

  QStringList files;
  for(const QModelIndex& index : selection) {
    files.push_back(_model->data(index, Qt::EditRole).toString());
  }

  _model->remove(files);
}
