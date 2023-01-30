}
static int LoadClip( demux_t *p_demux )
{
    return Load( p_demux, "CLIPINF",
                 p_demux->p_sys->b_shortname ? FilterClpiShort : FilterClpiLong, LoadClpi );
}
