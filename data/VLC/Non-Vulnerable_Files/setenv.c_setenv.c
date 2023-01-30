#include <string.h>
int setenv (const char *name, const char *value, int override)
{
#ifdef HAVE_GETENV
    if (override == 0 && getenv (name) != NULL)
        return 0;
    size_t namelen = strlen (name);
    size_t valuelen = strlen (value);
    char *var = malloc (namelen + valuelen + 2);
    if (var == NULL)
        return -1;
    sprintf (var, "%s=%s", name, value);
    /* This leaks memory. This is unavoidable. */
    return putenv (var);
#else
    return -1;
#endif
}
