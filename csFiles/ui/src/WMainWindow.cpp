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
#include <QtCore/QProcess>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>

#include "WMainWindow.h"
#include "ui_WMainWindow.h"

#include "Settings.h"
#include "WFind.h"
#include "WGrep.h"

////// public ////////////////////////////////////////////////////////////////

WMainWindow::WMainWindow(QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , ui{new Ui::WMainWindow}
{
  ui->setupUi(this);

  // Tab Widget //////////////////////////////////////////////////////////////

  connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &WMainWindow::removeTab);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->quitAction, &QAction::triggered, this, &WMainWindow::close);

  connect(ui->newFindAction,  &QAction::triggered, this, &WMainWindow::newFindTab);
  connect(ui->newGrepAction,  &QAction::triggered, this, &WMainWindow::newGrepTab);
  connect(ui->closeTabAction, &QAction::triggered, this, &WMainWindow::closeTab);
  connect(ui->closeAllAction, &QAction::triggered, this, &WMainWindow::closeAllTabs);
}

WMainWindow::~WMainWindow()
{
  closeAllTabs();
  delete ui;
}

////// private slots /////////////////////////////////////////////////////////

void WMainWindow::closeAllTabs()
{
  while( ui->tabWidget->count() > 0 ) {
    removeTab(0);
  }
}

void WMainWindow::closeTab()
{
  if( ui->tabWidget->count() > 0 ) {
    removeTab(ui->tabWidget->currentIndex());
  }
}

void WMainWindow::editFile(const QString& filename, int line)
{
  if( filename.isEmpty()  ||  !QFileInfo(filename).isFile() ) {
    return;
  }

  const QString quotedFilename = QStringLiteral("\"%1\"").arg(filename);
  const QString        lineStr = QString::number(qMax<int>(1, line));

  QString args = Settings::global::editorArgs;
  args.replace(QStringLiteral("%F"), quotedFilename);
  args.replace(QStringLiteral("%L"), lineStr);

  const QString cmd = QStringLiteral("\"%1\" %2").arg(Settings::global::editorExec).arg(args);
  QProcess::startDetached(cmd);
}

void WMainWindow::grepFiles(const QString& rootPath, const QStringList& files)
{
  if( files.isEmpty() ) {
    return;
  }

  newGrepTab();
  WGrep *grep = dynamic_cast<WGrep*>(ui->tabWidget->currentWidget());
  if( grep == nullptr ) {
    return;
  }

  if( rootPath.isEmpty() ) {
    grep->appendFiles(files);
  } else {
    grep->appendFiles(rootPath, files);
  }
}

void WMainWindow::newFindTab()
{
  addTabWidget(new WFind);
}

void WMainWindow::newGrepTab()
{
  addTabWidget(new WGrep);
}

void WMainWindow::openLocation(const QString& s)
{
  if( s.isEmpty() ) {
    return;
  }
  const QFileInfo info(s);
  const QString path = info.isDir()
      ? info.absoluteFilePath()
      : info.absolutePath();

  QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void WMainWindow::removeTab(int index)
{
  QWidget *w = ui->tabWidget->widget(index);
  if( w == nullptr ) {
    return;
  }
  ui->tabWidget->removeTab(index);
  delete w;
}

void WMainWindow::setTabLabel(const QString& text)
{
  const int index = ui->tabWidget->indexOf(qobject_cast<QWidget*>(sender()));
  if( index < 0 ) {
    return;
  }
  ui->tabWidget->setTabText(index, text);
}

////// private ///////////////////////////////////////////////////////////////

void WMainWindow::addTabWidget(ITabWidget *tabWidget)
{
  if( tabWidget == nullptr ) {
    return;
  }

  const int index = ui->tabWidget->addTab(tabWidget, tabWidget->tabLabelBase());
  ui->tabWidget->setCurrentIndex(index);

  connect(tabWidget, &ITabWidget::editFileRequested,     this, &WMainWindow::editFile);
  connect(tabWidget, &ITabWidget::grepRequested,         this, &WMainWindow::grepFiles);
  connect(tabWidget, &ITabWidget::openLocationRequested, this, &WMainWindow::openLocation);
  connect(tabWidget, &ITabWidget::tabLabelChanged,       this, &WMainWindow::setTabLabel);
}
