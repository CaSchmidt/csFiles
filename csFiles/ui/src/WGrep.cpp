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

#include <QtConcurrent/QtConcurrentMap>
#include <QtWidgets/QMessageBox>

#include <csQt/csTreeModel.h>

#include "MatchResultsModel.h"
#include "ResultsProxyDelegate.h"
#include "WProgressLogger.h"

#include "WGrep.h"
#include "ui_WGrep.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  MatchJob makeJob(const QString& filename, const csILogger *logger, const Ui::WGrep *ui)
  {
    MatchJob job{filename};

    job.ignoreCase  = ui->ignoreCaseCheck->isChecked();
    job.logger      = logger;
    job.matchRegExp = ui->matchRegExpCheck->isChecked();
    job.pattern     = ui->patternEdit->text();

    return job;
  }

  void prepareResults(MatchResults& results)
  {
    // (1.1) Remove results without any matched lines ////////////////////////

    MatchResults::iterator last =
        std::remove_if(results.begin(), results.end(),
                       [](const MatchResult& r) -> bool {
      return r.isEmpty();
    });

    // (1.2) Remove range of "empty" results /////////////////////////////////

    results.erase(last, results.end());

    // (2) Sort results by filename //////////////////////////////////////////

    std::sort(results.begin(), results.end());

    // (3) Sort lines of each result /////////////////////////////////////////

    std::for_each(results.begin(), results.end(), [](MatchResult& r) -> void {
      std::sort(r.lines.begin(), r.lines.end());
    });
  }

  MatchResultsRoot *makeResults(MatchResults results, const QString& rootPath)
  {
    prepareResults(results);

    MatchResultsRoot *root = new MatchResultsRoot(rootPath);

    for(const MatchResult& result : results) {
      MatchResultsFile *file = new MatchResultsFile(result.filename, root);
      root->appendChild(file);

      for(const MatchedLine& mline : result.lines) {
        MatchResultsLine *line = new MatchResultsLine(mline, file);
        file->appendChild(line);
      }
    }

    return root;
  }

} // namespace priv

////// public ////////////////////////////////////////////////////////////////

WGrep::WGrep(QWidget *parent, Qt::WindowFlags f)
  : ITabWidget(parent, f)
  , ui{new Ui::WGrep}
{
  ui->setupUi(this);

  // User Interface //////////////////////////////////////////////////////////

  ui->filesWidget->setAutoRoot(true);

  new ResultsProxyDelegate(ui->resultsView);

  // Results Model ///////////////////////////////////////////////////////////

  _resultsModel = new csTreeModel(new MatchResultsRoot(QString()), this);
  ui->resultsView->setModel(_resultsModel);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->grepButton, &QPushButton::clicked, this, &WGrep::executeGrep);
  connect(ui->patternEdit, &QLineEdit::textChanged, this, &WGrep::setTabLabel);
}

WGrep::~WGrep()
{
  delete ui;
}

QString WGrep::tabLabelBase() const
{
  return tr("grep");
}

////// private slots /////////////////////////////////////////////////////////

void WGrep::clearResults()
{
  _resultsModel->setRoot(new MatchResultsRoot(QString()));
}

void WGrep::executeGrep()
{
  if( ui->filesWidget->count() < 1  ||  !tryCompile() ) {
    return;
  }

  clearResults();

  WProgressLogger dialog(this);
  dialog.setWindowTitle(tr("Executing grep..."));

  MatchJobs jobs;
  const QStringList files = ui->filesWidget->files();
  for(const QString& filename : files) {
    jobs.push_back(priv::makeJob(filename, dialog.logger(), ui));
  }

  QFutureWatcher<MatchResult> watcher;
  dialog.setFutureWatcher(&watcher);

  QFuture<MatchResult> future = QtConcurrent::mapped(jobs, executeJob);
  watcher.setFuture(future);

  dialog.exec();
  future.waitForFinished();

  _resultsModel->setRoot(priv::makeResults(future.results(), ui->filesWidget->rootPath()));
}

void WGrep::setTabLabel(const QString& text)
{
  if( text.isEmpty() ) {
    emit tabLabelChanged(tabLabelBase());
  } else {
    const QString label(QStringLiteral("%1 - [ %2 ]").arg(tabLabelBase()).arg(text));
    emit tabLabelChanged(label);
  }
}

////// private ///////////////////////////////////////////////////////////////

bool WGrep::tryCompile()
{
  if( ui->patternEdit->text().isEmpty() ) {
    return false;
  }

  IMatcherPtr matcher = createDefaultMatcher();
  if( !matcher ) {
    QMessageBox::critical(this, tr("Error"), tr("Creation of matcher failed!"),
                          QMessageBox::Ok, QMessageBox::Ok);
    return false;
  }

  matcher->setIgnoreCase(ui->ignoreCaseCheck->isChecked());
  matcher->setMatchRegExp(ui->matchRegExpCheck->isChecked());

  if( !matcher->compile(ui->patternEdit->text().toStdString()) ) {
    const QString error = QString::fromStdString(matcher->error());
    QMessageBox::critical(this, tr("Error"), error,
                          QMessageBox::Ok, QMessageBox::Ok);
    return false;
  }

  return true;
}
