}
static int SetChapter( demux_t *p_demux, int i_chapter )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const int i_title = p_demux->info.i_title;
    const input_title_t *p_title = p_sys->pp_title[i_title];
    if( i_chapter < 0 || i_chapter > p_title->i_seekpoint )
        return VLC_EGENERIC;
    if( SetTime( p_demux, p_title->seekpoint[i_chapter]->i_time_offset ) )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
