#include <errno.h>
static int check_align (size_t align)
{
    for (size_t i = sizeof (void *); i != 0; i *= 2)
        if (align == i)
            return 0;
    return EINVAL;
}
