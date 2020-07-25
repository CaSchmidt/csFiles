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

#include "WListEditor.h"
#include "ui_WListEditor.h"

////// public ////////////////////////////////////////////////////////////////

WListEditor::WListEditor(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , ui{new Ui::WListEditor}
{
  ui->setupUi(this);

  // Signals & Slots /////////////////////////////////////////////////////////

  connect(ui->addButton,    &QPushButton::clicked, this, &WListEditor::onAdd);
  connect(ui->downButton,   &QPushButton::clicked, this, &WListEditor::onDown);
  connect(ui->removeButton, &QPushButton::clicked, this, &WListEditor::onRemove);
  connect(ui->upButton,     &QPushButton::clicked, this, &WListEditor::onUp);
}

WListEditor::~WListEditor()
{
  delete ui;
}

////// protected /////////////////////////////////////////////////////////////

QPushButton *WListEditor::button(const Button id)
{
  if(        id == Add ) {
    return ui->addButton;
  } else if( id == Down ) {
    return ui->downButton;
  } else if( id == Remove ) {
    return ui->removeButton;
  } else if( id == Up ) {
    return ui->upButton;
  }
  return nullptr;
}

QListView *WListEditor::view()
{
  return ui->listView;
}

////// private slots /////////////////////////////////////////////////////////

void WListEditor::onAdd()
{
}

void WListEditor::onDown()
{
}

void WListEditor::onRemove()
{
}

void WListEditor::onUp()
{
}
