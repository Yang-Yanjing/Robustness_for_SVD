}
static int LoadPlaylist( demux_t *p_demux )
{
    return Load( p_demux, "PLAYLIST",
                 p_demux->p_sys->b_shortname ? FilterMplsShort : FilterMplsLong, LoadMpls );
}
