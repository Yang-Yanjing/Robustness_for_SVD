}
int Export_M3U( vlc_object_t *p_this )
{
    playlist_export_t *p_export = (playlist_export_t *)p_this;
    msg_Dbg( p_export, "saving using M3U format");
    DoChildren( p_export, p_export->p_root, utf8_fprintf );
    return VLC_SUCCESS;
}
