#include <stdlib.h>
long long atoll (const char *str)
{
    return strtoll (str, NULL, 10);
}
