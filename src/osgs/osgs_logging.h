#include <stdio.h>

#define _OSGS_LOG_PREFIX "[osgs] "
#define _OSGS_LOG_NEWLINE "\n"

#define _OSGS_LOGINFO(msg, ...) \
        fprintf(stdout, _OSGS_LOG_PREFIX msg _OSGS_LOG_NEWLINE, __VA_ARGS__)

#define OSGS_LOGINFO(...) _OSGS_LOGINFO(__VA_ARGS__, "")

#define _OSGS_LOGERROR(msg, ...) \
        fprintf(stderr, _OSGS_LOG_PREFIX msg _OSGS_LOG_NEWLINE, __VA_ARGS__)

#define OSGS_LOGERROR(...) _OSGS_LOGERROR(__VA_ARGS__, "")
