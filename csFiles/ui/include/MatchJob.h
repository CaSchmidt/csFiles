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

#ifndef MATCHJOB_H
#define MATCHJOB_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "IMatcher.h"

class csILogger;

////// MatchJob //////////////////////////////////////////////////////////////

struct MatchJob {
  MatchJob() noexcept = default;
  MatchJob(const MatchJob& other) noexcept;
  MatchJob(const QString& _filename) noexcept;

  QString filename{};
  const csILogger *logger{nullptr};
  IMatcherPtr matcher{};
};

using MatchJobs = QList<MatchJob>;

////// MatchedLine ///////////////////////////////////////////////////////////

struct MatchedLine {
  MatchedLine() noexcept = default;

  bool assign(const TextLine& text, const int lineno, const MatchList& matches);

  QString      text{};
  int          number{};
  QVector<int> start{};
  QVector<int> length{};
};

bool operator<(const MatchedLine& a, const MatchedLine& b);

using MatchedLines = QList<MatchedLine>;

////// MatchResult ///////////////////////////////////////////////////////////

struct MatchResult {
  MatchResult() noexcept = default;
  MatchResult(const MatchJob& job) noexcept;

  bool isEmpty() const;

  QString      filename{};
  MatchedLines lines{};
};

bool operator<(const MatchResult& a, const MatchResult& b);

using MatchResults = QList<MatchResult>;

////// Functions /////////////////////////////////////////////////////////////

MatchResult executeJob(const MatchJob& job);

#endif // MATCHJOB_H
