#include "config/configuration.h"
char *config_GetLibDir (void)
{
    char *path = NULL;
    /* Find the path to libvlc (i.e. ourselves) */
    FILE *maps = fopen ("/proc/self/maps", "rte");
    if (maps == NULL)
        goto error;
    char *line = NULL;
    size_t linelen = 0;
    uintptr_t needle = (uintptr_t)config_GetLibDir;
    for (;;)
    {
        ssize_t len = getline (&line, &linelen, maps);
        if (len == -1)
            break;
        void *start, *end;
        if (sscanf (line, "%p-%p", &start, &end) < 2)
            continue;
        /* This mapping contains the address of this function. */
        if (needle < (uintptr_t)start || (uintptr_t)end <= needle)
            continue;
        char *dir = strchr (line, '/');
        if (dir == NULL)
            continue;
        char *file = strrchr (line, '/');
        if (end == NULL)
            continue;
        *file = '\0';
        if (asprintf (&path, "%s/"PACKAGE, dir) == -1)
            path = NULL;
        break;
    }
    free (line);
    fclose (maps);
error:
    return (path != NULL) ? path : strdup (PKGLIBDIR);
}
