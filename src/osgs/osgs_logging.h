#include <stdio.h>

#define _OSGS_LOG_PREFIX "[osgs] %s:%d "
#define _OSGS_ERRLOG_PREFIX "[error:osgs] %s:%d "
#define _OSGS_LOG_NEWLINE "\n"

/**
 *  \brief OSGS Logging Function, provide arguments within an extra pair of (). Formatting supported.
 */
#define OSGS_LOGINFO(args) (fprintf(stdout, _OSGS_LOG_PREFIX, __FILE__, __LINE__), printf args, printf(_OSGS_LOG_NEWLINE))
/**
 *  \brief OSGS Error Logging Function, provide arguments within an extra pair of (). Formatting supported.
 */
#define OSGS_LOGERROR(args) (fprintf(stderr, _OSGS_ERRLOG_PREFIX, __FILE__, __LINE__), printf args, printf(_OSGS_LOG_NEWLINE))
