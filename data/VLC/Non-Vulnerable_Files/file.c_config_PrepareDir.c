}
static int config_PrepareDir (vlc_object_t *obj)
{
    char *psz_configdir = config_GetUserDir (VLC_CONFIG_DIR);
    if (psz_configdir == NULL)
        return -1;
    int ret = config_CreateDir (obj, psz_configdir);
    free (psz_configdir);
    return ret;
}
