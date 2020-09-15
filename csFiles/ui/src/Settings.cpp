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

#include "PatternList.h"

namespace Settings {

  // Settings ////////////////////////////////////////////////////////////////

  namespace global {

    QString editorExec(QStringLiteral("notepad.exe"));
    QString editorArgs(QStringLiteral("%F"));

  } // namespace global

  namespace find {

    Presets extensions;

  } // namespace find

  namespace grep {

    bool copyLocationDisplayName{false};

  } // namespace grep

  // Functions ///////////////////////////////////////////////////////////////

  void load()
  {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("csLabs"), QStringLiteral("csFiles"));

    // global ////////////////////////////////////////////////////////////////

    settings.beginGroup(QStringLiteral("global"));
    global::editorExec = settings.value(QStringLiteral("editor_exec"), global::editorExec).toString();
    global::editorArgs = settings.value(QStringLiteral("editor_args"), global::editorArgs).toString();
    settings.endGroup();

    // find //////////////////////////////////////////////////////////////////

    settings.beginGroup(QStringLiteral("find_extensions"));
    int i = 0;
    while( true ) {
      const QString  name = settings.value(QStringLiteral("name_%1").arg(i), QString()).toString();
      const QString value = settings.value(QStringLiteral("value_%1").arg(i), QString()).toString();

      if( name.isEmpty()  ||  value.isEmpty() ) {
        break;
      }

      find::extensions.push_back(Preset(name.simplified(), cleanPatternList(value)));

      i++;
    }
    settings.endGroup();

    // grep //////////////////////////////////////////////////////////////////

    settings.beginGroup(QStringLiteral("grep"));
    grep::copyLocationDisplayName = settings.value(QStringLiteral("copy_location_displayname"), grep::copyLocationDisplayName).toBool();
    settings.endGroup();
  }

  void save()
  {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("csLabs"), QStringLiteral("csFiles"));
    settings.clear();

    // global ////////////////////////////////////////////////////////////////

    settings.beginGroup(QStringLiteral("global"));
    settings.setValue(QStringLiteral("editor_exec"), global::editorExec);
    settings.setValue(QStringLiteral("editor_args"), global::editorArgs);
    settings.endGroup();

    // find //////////////////////////////////////////////////////////////////

    if( !find::extensions.isEmpty() ) {
      settings.beginGroup(QStringLiteral("find_extensions"));
      for(int i = 0; i < find::extensions.size(); i++) {
        settings.setValue(QStringLiteral("name_%1").arg(i), find::extensions.at(i).name.simplified());
        settings.setValue(QStringLiteral("value_%1").arg(i), cleanPatternList(find::extensions.at(i).value));
      }
      settings.endGroup();
    }

    // grep //////////////////////////////////////////////////////////////////

    settings.beginGroup(QStringLiteral("grep"));
    settings.setValue(QStringLiteral("copy_location_displayname"), grep::copyLocationDisplayName);
    settings.endGroup();

    //////////////////////////////////////////////////////////////////////////

    settings.sync();
  }

} // namespace Settings
