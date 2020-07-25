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

#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>

#include "WFileList.h"

#include "FilesModel.h"

////// public ////////////////////////////////////////////////////////////////

WFileList::WFileList(QWidget *parent, Qt::WindowFlags f)
  : WListEditor(parent, f)
{
  // User Interface //////////////////////////////////////////////////////////

  button(Up)->setVisible(false);
  button(Down)->setVisible(false);

  view()->setAlternatingRowColors(true);
  view()->setContextMenuPolicy(Qt::CustomContextMenu);
  view()->setEditTriggers(QAbstractItemView::NoEditTriggers);
  view()->setSelectionBehavior(QAbstractItemView::SelectRows);
  view()->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // Model ///////////////////////////////////////////////////////////////////

  _model = new FilesModel(this);
  view()->setModel(_model);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(view(), &QListView::customContextMenuRequested, this, &WFileList::showContextMenu);
}

WFileList::~WFileList()
{
}

void WFileList::setEnabledButtons(const bool add_on, const bool remove_on)
{
  button(Add)->setEnabled(add_on);
  button(Remove)->setEnabled(remove_on);
}

QString WFileList::selectionFilter() const
{
  return _filter;
}

void WFileList::setSelectionFilter(const QString& filter)
{
  _filter = filter;
}

void WFileList::clearFiles()
{
  _model->clear();
}

QStringList WFileList::files() const
{
  return _model->files();
}

void WFileList::appendFiles(const QStringList& files)
{
  _model->append(files);
}

void WFileList::appendFiles(const QDir& root, const QStringList& files)
{
  _model->append(files);
  _model->setRoot(root);
}

void WFileList::appendFiles(const QString& rootPath, const QStringList& files)
{
  appendFiles(QDir(rootPath), files);
}

////// public ////////////////////////////////////////////////////////////////

void WFileList::showContextMenu(const QPoint& pos)
{
  QMenu menu;
  QAction *copy = menu.addAction(tr("Copy list"));
  menu.addSeparator();
  QAction *clear = menu.addAction(tr("Clear list"));
  menu.addSeparator();
  QAction *clearRoot = menu.addAction(tr("Clear root"));
  QAction *choice = menu.exec(view()->mapToGlobal(pos));
  if( choice == nullptr ) {
    return;
  }
  if(        choice == copy ) {
    QStringList files = _model->files();
    if( !files.empty() ) {
      qSort(files);
      QGuiApplication::clipboard()->setText(files.join(QChar::fromLatin1('\n')));
    }
  } else if( choice == clearRoot ) {
    _model->clearRoot();
  } else if( choice == clear ) {
    _model->clear();
  }
}

////// private ///////////////////////////////////////////////////////////////

void WFileList::onAdd()
{
  const QStringList files =
      QFileDialog::getOpenFileNames(this, tr("Select files"), QDir::currentPath(), _filter.isEmpty()
                                    ? tr("All files (*.*)")
                                    : _filter);
  if( files.isEmpty() ) {
    return;
  }
  const QString path = QFileInfo(files.front()).absolutePath();
  _model->append(files);
  _model->setRoot(path);
  QDir::setCurrent(path);
}

void WFileList::onRemove()
{
  const QModelIndexList selection = view()->selectionModel()->selectedIndexes();

  QStringList files;
  for(const QModelIndex& index : selection) {
    const QString file = _model->data(index, Qt::EditRole).toString();
    files.push_back(file);
  }

  _model->remove(files);
}
