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

#include <QtCore/QSettings>

#include "Settings.h"

namespace Settings {

  // Settings ////////////////////////////////////////////////////////////////

  QString editorExec(QStringLiteral("notepad.exe"));
  QString editorArgs(QStringLiteral("%F"));

  Presets extensions;

  // Functions ///////////////////////////////////////////////////////////////

  QString cleanList(const QString& s)
  {
    return prepareList(s).join(QStringLiteral(", "));
  }

  QStringList prepareList(QString s)
  {
    s.remove(QRegExp(QStringLiteral("[^,0-9a-z]"), Qt::CaseInsensitive));
    return s.split(QChar::fromLatin1(','), QString::SkipEmptyParts);
  }

  void load()
  {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("csLabs"), QStringLiteral("csFiles"));

    settings.beginGroup(QStringLiteral("editor"));
    editorExec = settings.value(QStringLiteral("exec"), editorExec).toString();
    editorArgs = settings.value(QStringLiteral("args"), editorArgs).toString();
    settings.endGroup();

    settings.beginGroup(QStringLiteral("extensions"));
    int i = 0;
    while( true ) {
      const QString  name = settings.value(QStringLiteral("name_%1").arg(i), QString()).toString();
      const QString value = settings.value(QStringLiteral("value_%1").arg(i), QString()).toString();

      if( name.isEmpty()  ||  value.isEmpty() ) {
        break;
      }

      extensions.push_back(Preset(name.simplified(), cleanList(value)));

      i++;
    }
    settings.endGroup();
  }

  void save()
  {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("csLabs"), QStringLiteral("csFiles"));
    settings.clear();

    settings.beginGroup(QStringLiteral("editor"));
    settings.setValue(QStringLiteral("exec"), editorExec);
    settings.setValue(QStringLiteral("args"), editorArgs);
    settings.endGroup();

    if( !extensions.isEmpty() ) {
      settings.beginGroup(QStringLiteral("extensions"));
      for(int i = 0; i < extensions.size(); i++) {
        settings.setValue(QStringLiteral("name_%1").arg(i), extensions.at(i).name.simplified());
        settings.setValue(QStringLiteral("value_%1").arg(i), cleanList(extensions.at(i).value));
      }
      settings.endGroup();
    }

    //////////////////////////////////////////////////////////////////////////

    settings.sync();
  }

} // namespace Settings
