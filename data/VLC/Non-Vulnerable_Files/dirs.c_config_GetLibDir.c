#include <limits.h>
char *config_GetLibDir (void)
{
#if VLC_WINSTORE_APP
    return NULL;
#else
    /* Get our full path */
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery (config_GetLibDir, &mbi, sizeof(mbi)))
        goto error;
    wchar_t wpath[MAX_PATH];
    if (!GetModuleFileName ((HMODULE) mbi.AllocationBase, wpath, MAX_PATH))
        goto error;
    wchar_t *file = wcsrchr (wpath, L'\\');
    if (file == NULL)
        goto error;
    *file = L'\0';
    return FromWide (wpath);
error:
    abort ();
#endif
}
