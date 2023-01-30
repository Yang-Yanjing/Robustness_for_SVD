}
int Export_M3U8( vlc_object_t *p_this )
{
    playlist_export_t *p_export = (playlist_export_t *)p_this;
    msg_Dbg( p_export, "saving using M3U8 format");
    DoChildren( p_export, p_export->p_root, fprintf );
    return VLC_SUCCESS;
}
