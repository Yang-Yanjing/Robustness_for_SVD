#warning FIXME Use known folders on Vista and above
char *config_GetUserDir (vlc_userdir_t type)
{
    switch (type)
    {
        case VLC_HOME_DIR:
            return config_GetShellDir (CSIDL_PERSONAL);
        case VLC_CONFIG_DIR:
        case VLC_DATA_DIR:
        case VLC_CACHE_DIR:
            return config_GetAppDir ();
        case VLC_DESKTOP_DIR:
        case VLC_DOWNLOAD_DIR:
        case VLC_TEMPLATES_DIR:
        case VLC_PUBLICSHARE_DIR:
        case VLC_DOCUMENTS_DIR:
            return config_GetUserDir(VLC_HOME_DIR);
        case VLC_MUSIC_DIR:
            return config_GetShellDir (CSIDL_MYMUSIC);
        case VLC_PICTURES_DIR:
            return config_GetShellDir (CSIDL_MYPICTURES);
        case VLC_VIDEOS_DIR:
            return config_GetShellDir (CSIDL_MYVIDEO);
    }
    assert (0);
}
