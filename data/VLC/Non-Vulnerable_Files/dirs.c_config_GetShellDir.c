}
static char *config_GetShellDir (int csidl)
{
    wchar_t wdir[MAX_PATH];
    if (SHGetFolderPathW (NULL, csidl | CSIDL_FLAG_CREATE,
                          NULL, SHGFP_TYPE_CURRENT, wdir ) == S_OK)
        return FromWide (wdir);
    return NULL;
}
