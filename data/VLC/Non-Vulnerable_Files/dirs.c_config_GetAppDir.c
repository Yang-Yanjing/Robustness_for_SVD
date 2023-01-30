}
static char *config_GetAppDir (void)
{
    char *psz_dir;
    char *psz_parent = config_GetShellDir (CSIDL_APPDATA);
    if (psz_parent == NULL
     ||  asprintf (&psz_dir, "%s\\vlc", psz_parent) == -1)
        psz_dir = NULL;
    free (psz_parent);
    return psz_dir;
}
