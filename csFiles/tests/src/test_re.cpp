#include <cstdio>
#include <cstdlib>

#include "IMatcher.h"

#undef HAVE_REGEXP

void run_re_tests()
{
  IMatcherPtr rx = createPcre2Matcher();

#ifdef HAVE_REGEXP
  const std::string pattern("\\d{3}abc");
#else
  const std::string pattern("789abc");
#endif
  const std::string subject("0123456789abcdef<{[()]}>0123456789ABCDEF");

#ifdef HAVE_REGEXP
  rx->setMatchRegExp(true);
#endif
  printf("matchRegExp(): %s\n", rx->matchRegExp() ? "yes" : "no");
  rx->compile(pattern);
  if( rx->isError() ) {
    printf("error: %s\n", rx->error().data());
  }
  printf("\"%s\" matches \"%s\": %s\n",
         pattern.data(), subject.data(), rx->match(subject)
         ? "yes"
         : "no");
  if( rx->hasMatch() ) {
    const MatchList m = rx->getMatch();
    printf("start = %d, length = %d\n", m.back().first, m.back().second);
  }
  fflush(stdout);
}
