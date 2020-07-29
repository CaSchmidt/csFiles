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

#include <QtCore/QDirIterator>
#include <QtWidgets/QFileDialog>

#include "WFind.h"
#include "ui_WFind.h"

#include "FilesModel.h"

////// Constants /////////////////////////////////////////////////////////////

constexpr int kMaxTabLabel = 32;

////// public ////////////////////////////////////////////////////////////////

WFind::WFind(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui(new Ui::WFind)
{
  ui->setupUi(this);

  // Results Model ///////////////////////////////////////////////////////////

  _model = new FilesModel(this);
  ui->resultsView->setModel(_model);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->browseButton, &QPushButton::clicked, this, &WFind::browse);
  connect(ui->dirEdit, &QLineEdit::textChanged, this, &WFind::setTabLabel);
  connect(ui->findButton, &QPushButton::clicked, this, &WFind::executeFind);
}

WFind::~WFind()
{
  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WFind::browse()
{
  const QString path =
      QFileDialog::getExistingDirectory(this, tr("Browse Directory"), QDir::currentPath());
  if( path.isEmpty() ) {
    return;
  }
  ui->dirEdit->setText(path);
  QDir::setCurrent(path);
}

void WFind::executeFind()
{
  if( ui->dirEdit->text().isEmpty() ) {
    return;
  }

  _model->clear();

  const QDir rootDir(ui->dirEdit->text());
  _model->setRoot(rootDir);

  const bool no_filter = !ui->dirsCheck->isChecked()  &&  !ui->filesCheck->isChecked();

  QDir::Filters dirFilters{0};
  dirFilters.setFlag(QDir::NoDot, true);
  dirFilters.setFlag(QDir::NoDotDot, true);
  dirFilters.setFlag(QDir::Dirs,  no_filter  ||  ui->dirsCheck->isChecked());
  dirFilters.setFlag(QDir::Files, no_filter  ||  ui->filesCheck->isChecked());

  QDirIterator::IteratorFlags iterFlags = QDirIterator::NoIteratorFlags;
  iterFlags.setFlag(QDirIterator::FollowSymlinks, ui->followSymLinkCheck->isChecked());
  iterFlags.setFlag(QDirIterator::Subdirectories, ui->subDirsCheck->isChecked());

  QDirIterator iter(rootDir.absolutePath(), dirFilters, iterFlags);

  QStringList results;
  while( iter.hasNext() ) {
    results.push_back(iter.next());
  }
  _model->append(results);
}

void WFind::setTabLabel(const QString& text)
{
  const QString baseLabel(tr("find"));
  if( text.isEmpty() ) {
    emit tabLabelChanged(baseLabel);
  } else {
    QString path(text);
    if( path.size() > kMaxTabLabel ) {
      const int pos = path.lastIndexOf(QChar::fromLatin1('/'));
      if( pos >= 0 ) {
        path.remove(0, pos);
        path.insert(0, QString::fromUtf8("…"));
      }
    }
    emit tabLabelChanged(QStringLiteral("%1 - [ %2 ]").arg(baseLabel).arg(path));
  }
}
