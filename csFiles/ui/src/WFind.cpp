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
#include <QtCore/QRegExp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "WFind.h"
#include "ui_WFind.h"

#include "ExtensionFilter.h"
#include "FilenameFilter.h"
#include "FilesModel.h"
#include "PathFilter.h"
#include "PatternList.h"

////// Constants /////////////////////////////////////////////////////////////

constexpr int kMaxTabLabel = 32;

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  QDir::Filters makeDirFilters(const Ui::WFind *ui)
  {
    const bool no_filter = !ui->dirsCheck->isChecked()  &&  !ui->filesCheck->isChecked();

    QDir::Filters result{0};
    result.setFlag(QDir::NoDot, true);
    result.setFlag(QDir::NoDotDot, true);
    result.setFlag(QDir::Dirs,  no_filter  ||  ui->dirsCheck->isChecked());
    result.setFlag(QDir::Files, no_filter  ||  ui->filesCheck->isChecked());

    return result;
  }

  IFindFilterPtr makeExtensionFilter(Ui::WFind *ui)
  {
    ui->extensionFilterEdit->setText(cleanPatternList(ui->extensionFilterEdit->text()));
    return ExtensionFilter::create(ui->extensionFilterEdit->text(), ui->extensionRejectCheck->isChecked());
  }

  IFindFilterPtr makeFilenameFilter(const Ui::WFind *ui)
  {
    return FilenameFilter::create(ui->filenameFilterEdit->text(), ui->filenameRejectCheck->isChecked());
  }

  QDirIterator::IteratorFlags makeIterFlags(const Ui::WFind *ui)
  {
    QDirIterator::IteratorFlags result = QDirIterator::NoIteratorFlags;
    result.setFlag(QDirIterator::FollowSymlinks, ui->followSymLinkCheck->isChecked());
    result.setFlag(QDirIterator::Subdirectories, ui->subDirsCheck->isChecked());

    return result;
  }

  IFindFilterPtr makePathFilter(Ui::WFind *ui)
  {
    ui->pathFilterEdit->setText(cleanPatternList(ui->pathFilterEdit->text()));
    return PathFilter::create(ui->pathFilterEdit->text(), ui->pathRejectCheck->isChecked());
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

WFind::WFind(QWidget *parent, Qt::WindowFlags f)
  : ITabWidget(parent, f)
  , ui(new Ui::WFind)
{
  ui->setupUi(this);

  // User Interface //////////////////////////////////////////////////////////

  ui->resultsView->setContextMenuPolicy(Qt::CustomContextMenu);

  // Results Model ///////////////////////////////////////////////////////////

  _resultsModel = new FilesModel(this);
  ui->resultsView->setModel(_resultsModel);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->browseButton, &QPushButton::clicked, this, &WFind::browse);
  connect(ui->dirEdit, &QLineEdit::textChanged, this, &WFind::setTabLabel);
  connect(ui->findButton, &QPushButton::clicked, this, &WFind::executeFind);
  connect(ui->resultsView, &QListView::customContextMenuRequested, this, &WFind::showContextMenu);
}

WFind::~WFind()
{
  delete ui;
}

QString WFind::tabLabelBase() const
{
  return tr("find");
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

void WFind::clearResults()
{
  _resultsModel->clear();
}

void WFind::executeFind()
{
  if( ui->dirEdit->text().isEmpty() ) {
    return;
  }

  _resultsModel->clear();

  const QDir rootDir(ui->dirEdit->text());
  _resultsModel->setRoot(rootDir);

  const IFindFilterPtr  extFilter = priv::makeExtensionFilter(ui);
  const IFindFilterPtr fileFilter = priv::makeFilenameFilter(ui);
  const IFindFilterPtr pathFitler = priv::makePathFilter(ui);

  const QDir::Filters              dirFilters = priv::makeDirFilters(ui);
  const QDirIterator::IteratorFlags iterFlags = priv::makeIterFlags(ui);

  QDirIterator iter(rootDir.absolutePath(), dirFilters, iterFlags);

  QStringList results;
  while( iter.hasNext() ) {
    iter.next();
    const QFileInfo info = iter.fileInfo();

    if( pathFitler->filtered(info) ) {
      continue;
    }
    if( extFilter->filtered(info) ) {
      continue;
    }
    if( fileFilter->filtered(info) ) {
      continue;
    }

    results.push_back(info.absoluteFilePath());
  }
  _resultsModel->append(results);
}

void WFind::setTabLabel(const QString& text)
{
  if( text.isEmpty() ) {
    emit tabLabelChanged(tabLabelBase());
  } else {
    QString path(text);
    if( path.size() > kMaxTabLabel ) {
      const int pos = path.lastIndexOf(QChar::fromLatin1('/'));
      if( pos >= 0 ) {
        path.remove(0, pos);
        path.insert(0, QString::fromUtf8("…"));
      }
    }
    emit tabLabelChanged(QStringLiteral("%1 - [ %2 ]").arg(tabLabelBase()).arg(path));
  }
}

void WFind::showContextMenu(const QPoint& p)
{
  QMenu menu;

  QAction  *grepAction = menu.addAction(tr("grep results"));
  menu.addSeparator();
  QAction *clearAction = menu.addAction(tr("Clear results"));

  QAction *choice = menu.exec(ui->resultsView->viewport()->mapToGlobal(p));
  if(        choice == nullptr ) {
    return;

  } else if( choice == grepAction ) {
    emit grepRequested(_resultsModel->rootPath(), _resultsModel->files());

  } else if( choice == clearAction ) {
    clearResults();

  }
}
