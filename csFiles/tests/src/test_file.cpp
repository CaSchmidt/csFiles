#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <list>
#include <string>

#include <QtCore/QFile>

#include "tests.h"

#include "TextBuffer.h"

using String     = std::string;
using StringList = std::list<String>;

bool equals(const StringList& l1, const StringList& l2)
{
  if( l1.size() != l2.size() ) {
    return false;
  }
  return std::equal(l1.cbegin(), l1.cend(), l2.cbegin(),
                    [](const String& a, const String& b) -> bool {
    if( a.size() != b.size() ) {
      return false;
    }
    return a.size() > 0
        ? a == b
        : true;
  });
}

void run_file(const char *filename, const StringList& ref)
{
  QFile *file = new QFile(QString::fromLatin1(filename));
  if( file == nullptr  ||  !file->open(QIODevice::ReadOnly) ) {
    delete file;
    printf("ERROR: Unable to open file \"%s\"!\n", filename);
    return;
  }
  TextBufferPtr buffer = TextBuffer::create(file);

  printf("EOL: %d\n", int(buffer->info().eolType()));

  int lineno = 0;
  StringList lines;
  while( buffer->hasNextLine() ) {
    bool ok = false;
    const TextLine line = buffer->nextLine(false, &ok);
    if( !ok  ||  !isValid(line) ) {
      printf("ERROR: ok = %s, isValid = %s\n",
             ok ? "true" : "false",
             isValid(line) ? "true" : "false");
      break;
    }
    const String str(line.first, line.second);
    printf("%d: %s\n", ++lineno, str.data());
    lines.push_back(str);

    if( lineno > 10 ) {
      break;
    }
  }

  printf("\"%s\": %s\n",
         filename,
         equals(lines, ref) ? "OK" : "not OK");
  printf("\n");
}

void run_file_tests()
{
  const StringList ref_incomplete{String{"0123"}, String{""}};
  const StringList ref_long{String{"0123"}, String{""}, String{"0123456789ABCDEF0123456789ABCDEF"}};
  const StringList ref_short{String{"0123"}, String{""}, String{"0123456789ABCDEF"}};

  run_file("../findgrep/testdata/simple1.crlf.txt", ref_short);
  run_file("../findgrep/testdata/simple2.crlf.txt", ref_short);
  run_file("../findgrep/testdata/fit.crlf.txt", ref_long);
  run_file("../findgrep/testdata/exceed.crlf.txt", ref_incomplete);

  fflush(stdout);
}
