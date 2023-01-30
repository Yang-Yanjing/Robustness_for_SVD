};
static int DemuxPause( demux_t *demux )
{
    demux_sys_t *p_sys = demux->p_sys;
    mtime_t now = mdate();
    if( now >= p_sys->end )
        return 0;
    msleep( 10000 ); /* FIXME!!! */
    return 1;
}
