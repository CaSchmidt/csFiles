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

#include <QtCore/QFile>

#include <csUtil/csILogger.h>

#include "IMatcher.h"
#include "TextBuffer.h"

#include "MatchJob.h"

////// Private ///////////////////////////////////////////////////////////////

namespace priv {

  void printText(const MatchJob& job, const QString& text)
  {
    if( job.logger == nullptr ) {
      return;
    }
    const QString s = QStringLiteral("%1: %2").arg(job.filename).arg(text);
    job.logger->logText(s.toStdString());
  }

  void printWarning(const MatchJob& job, const QString& warning)
  {
    if( job.logger == nullptr ) {
      return;
    }
    const QString s = QStringLiteral("%1: %2").arg(job.filename).arg(warning);
    job.logger->logWarning(s.toStdString());
  }

  void printError(const MatchJob& job, const QString& error)
  {
    if( job.logger == nullptr ) {
      return;
    }
    const QString s = QStringLiteral("%1: %2").arg(job.filename).arg(error);
    job.logger->logError(s.toStdString());
  }

  void printError(const MatchJob& job, const int lineno, const QString& error)
  {
    if( job.logger == nullptr ) {
      return;
    }
    const QString s = QStringLiteral("%1:%2: %3").arg(job.filename).arg(lineno).arg(error);
    job.logger->logError(s.toStdString());
  }

} // namespace priv

////// MatchJob - public /////////////////////////////////////////////////////

MatchJob::MatchJob(const MatchJob& other) noexcept
  : filename(other.filename)
  , logger{other.logger}
{
  if( other.matcher ) {
    matcher = other.matcher->clone();
  }
}

MatchJob::MatchJob(const QString& _filename) noexcept
  : filename{_filename}
{
}

////// MatchedLine - public //////////////////////////////////////////////////

bool MatchedLine::assign(const TextLine& text, const int lineno, const MatchList& matches)
{
  if( diff(text) < 1  ||  lineno < 1  ||  matches.empty() ) {
    return false;
  }

  MatchedLine::text = QString::fromUtf8(text.first, static_cast<int>(diff(text)));
  number = lineno;

  start.reserve(static_cast<int>(matches.size()));
  length.reserve(static_cast<int>(matches.size()));

  for(const Match& match : matches) {
    start.push_back(match.first);
    length.push_back(match.second);
  }

  return start.size() == static_cast<int>(matches.size())  &&  start.size() == length.size();
}

bool operator<(const MatchedLine& a, const MatchedLine& b)
{
  return a.number < b.number;
}

////// MatchResult - public //////////////////////////////////////////////////

MatchResult::MatchResult(const MatchJob& job) noexcept
  : filename{job.filename}
{
}

bool MatchResult::isEmpty() const
{
  return lines.isEmpty();
}

bool operator<(const MatchResult& a, const MatchResult& b)
{
  return a.filename < b.filename;
}

////// Public ////////////////////////////////////////////////////////////////

MatchResult executeJob(const MatchJob& job)
{
  MatchResult result(job);

  if( !job.matcher ) {
    priv::printError(job, QStringLiteral("No matcher set!"));
    return result;
  }

  QFile *file = new QFile(job.filename);
  if( file == nullptr  ||  !file->open(QIODevice::ReadOnly) ) {
    delete file;
    priv::printError(job, QStringLiteral("Unable to open file!"));
    return result;
  }

  TextBufferPtr buffer = TextBuffer::create(file);
  if( !buffer ) {
    priv::printError(job, QStringLiteral("Creation of TextBuffer failed!"));
    return result;
  }

  if( buffer->info().isBinary() ) {
    priv::printWarning(job, QStringLiteral("Ignoring binary file!"));
    return result;
  }

  if( buffer->info().eolType() == EndOfLine::Unknown ) {
    priv::printWarning(job, QStringLiteral("Ignoring file with indeterminable EOL type!"));
    return result;
  }

  if( !job.matcher->setEndOfLine(buffer->info().eolType()) ) {
    priv::printError(job, QStringLiteral("Unable to set EOL type!"));
    return result;
  }

  int lineno = 0;
  while( buffer->hasNextLine() ) {
    lineno++;

    bool ok = false;
    const TextLine text = buffer->nextLine(true, &ok);
    if( !ok  ||  !isValid(text) ) {
      priv::printError(job, lineno, QStringLiteral("Unable to extract line!"));
      return result;
    }

    if( !job.matcher->match(text.first, text.second) ) {
      continue;
    }

    MatchedLine line;
    if( !line.assign(buffer->info().removeEnding(text), lineno, job.matcher->getMatch()) ) {
      continue;
    }

    result.lines.push_back(line);
  }

  priv::printText(job, QStringLiteral("Done!"));

  return result;
}
