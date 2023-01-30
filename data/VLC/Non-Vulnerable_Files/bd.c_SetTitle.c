}
static int SetTitle( demux_t *p_demux, int i_title )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if( i_title < 0 || i_title >= p_sys->i_title )
        return VLC_EGENERIC;
    if( SetPlayItem( p_demux, i_title, 0 ) )
        return VLC_EGENERIC;
    /* */
    p_demux->info.i_update |= INPUT_UPDATE_TITLE | INPUT_UPDATE_SEEKPOINT;
    p_demux->info.i_title = i_title;
    p_demux->info.i_seekpoint = 0;
    return VLC_SUCCESS;
}
