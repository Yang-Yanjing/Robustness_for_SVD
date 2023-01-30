#include <locale.h>
char *setlocale (int cat, const char *locale)
{
    if (override && locale != NULL)
    {
        LOG("Blocked", "%d, \"%s\"", cat, locale);
        return NULL;
    }
    return CALL(setlocale, cat, locale);
}
