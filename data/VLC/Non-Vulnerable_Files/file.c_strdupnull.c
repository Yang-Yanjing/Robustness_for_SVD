#include "modules/modules.h"
static inline char *strdupnull (const char *src)
{
    return src ? strdup (src) : NULL;
}
