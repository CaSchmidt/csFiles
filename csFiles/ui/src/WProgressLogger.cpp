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

#include <QtWidgets/QPushButton>

#include <csQt/csDialogButtonBox.h>

#include "WProgressLogger.h"
#include "ui_WProgressLogger.h"

////// public ////////////////////////////////////////////////////////////////

WProgressLogger::WProgressLogger(QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
  , ui{new Ui::WProgressLogger}
{
  ui->setupUi(this);

  // User Interface //////////////////////////////////////////////////////////

  csRemoveAllButtons(ui->buttonBox);

  ui->buttonBox->addButton(QDialogButtonBox::Cancel);
  QPushButton *cancel = ui->buttonBox->button(QDialogButtonBox::Cancel);

  csRemoveDefaults(ui->buttonBox, true);

  // Signals & Slots /////////////////////////////////////////////////////////

  if( cancel != nullptr ) {    
    connect(cancel, &QPushButton::clicked, this, &WProgressLogger::canceled);

    connect(cancel, &QPushButton::clicked, this, &WProgressLogger::rejected);
    connect(cancel, &QPushButton::clicked, this, &WProgressLogger::reject);
  }

  connect(ui->progressBar, &QProgressBar::valueChanged, this, &WProgressLogger::valueChanged);
}

WProgressLogger::~WProgressLogger()
{
  delete ui;
}

const csILogger *WProgressLogger::logger() const
{
  return ui->logBrowser;
}

int WProgressLogger::maximum() const
{
  return ui->progressBar->maximum();
}

int WProgressLogger::minimum() const
{
  return ui->progressBar->minimum();
}

////// public slots //////////////////////////////////////////////////////////

void WProgressLogger::finish()
{
  csRemoveAllButtons(ui->buttonBox);

  ui->buttonBox->addButton(QDialogButtonBox::Close);
  QPushButton *close = ui->buttonBox->button(QDialogButtonBox::Close);
  if( close != nullptr ) {
    close->disconnect();

    connect(close, &QPushButton::clicked, this, &WProgressLogger::accepted);
    connect(close, &QPushButton::clicked, this, &WProgressLogger::accept);

    close->setAutoDefault(true);
    close->setDefault(true);
  }
}

void WProgressLogger::setMaximum(int maximum)
{
  ui->progressBar->setMaximum(maximum);
}

void WProgressLogger::setMinimum(int minimum)
{
  ui->progressBar->setMinimum(minimum);
}

void WProgressLogger::setRange(int minimum, int maximum)
{
  ui->progressBar->setRange(minimum, maximum);
}

void WProgressLogger::setValue(int value)
{
  ui->progressBar->setValue(value);
}
