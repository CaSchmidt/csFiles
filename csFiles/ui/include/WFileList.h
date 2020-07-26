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

#ifndef WFILELIST_H
#define WFILELIST_H

#include <csQt/csWListEditor.h>

class QDir;

class WFileList : public csWListEditor {
  Q_OBJECT
  Q_PROPERTY(bool autoRoot
             READ autoRoot
             WRITE setAutoRoot
             MEMBER _autoRoot
             NOTIFY autoRootChanged)
  Q_PROPERTY(QString selectionFilter
             READ selectionFilter
             WRITE setSelectionFilter
             MEMBER _selectionFilter
             NOTIFY selectionFilterChanged)
public:
  WFileList(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~WFileList();

  void setEnabledButtons(const bool add_on, const bool remove_on);

  bool autoRoot() const;
  void setAutoRoot(const bool on);

  QString selectionFilter() const;
  void setSelectionFilter(const QString& filter);

  QStringList files() const;

public slots:
  void appendFiles(const QStringList& files);
  void appendFiles(const QDir& root, const QStringList& files);
  void appendFiles(const QString& rootPath, const QStringList& files);
  void clearList();
  void clearRoot();
  void copyList();

private slots:
  void showContextMenu(const QPoint& pos);

private:
  void onAdd() final;
  void onRemove() final;

  bool _autoRoot{false};
  QString _selectionFilter{};
  class FilesModel *_model{nullptr};

signals:
  void autoRootChanged(bool);
  void selectionFilterChanged(const QString&);
};

#endif // WFILELIST_H
