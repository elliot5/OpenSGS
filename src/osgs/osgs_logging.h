#include <stdio.h>

#define _OSGS_LOG_PREFIX "[osgs] %s:%d "
#define _OSGS_LOG_NEWLINE "\n"

#define OSGS_LOGINFO(args) (fprintf(stdout, _OSGS_LOG_PREFIX, __FILE__, __LINE__), printf args, printf(_OSGS_LOG_NEWLINE))

#define OSGS_LOGERROR(args) (fprintf(stderr, _OSGS_LOG_PREFIX, __FILE__, __LINE__), printf args, printf(_OSGS_LOG_NEWLINE))



