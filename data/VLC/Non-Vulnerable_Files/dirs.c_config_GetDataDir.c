}
char *config_GetDataDir (void)
{
    const char *path = getenv ("VLC_DATA_PATH");
    return (path != NULL) ? strdup (path) : config_GetLibDir ();
}
