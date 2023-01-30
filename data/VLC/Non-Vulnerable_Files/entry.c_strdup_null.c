#include "libvlc.h"
static char *strdup_null (const char *str)
{
    return (str != NULL) ? strdup (str) : NULL;
}
