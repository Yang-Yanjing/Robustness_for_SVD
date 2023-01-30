}
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;
    demux_sys_t *p_sys;
    int         i_ret;
    if ( (i_ret = DemuxOpen( p_this )) != VLC_SUCCESS )
        return i_ret;
    /* Fill p_demux field */
    p_demux->pf_demux    = Demux;
    p_demux->pf_control  = Control;
    p_sys = p_demux->p_sys;
    p_sys->i_link = var_InheritInteger( p_demux, "linsys-sdi-link" );
    if( InitCapture( p_demux ) != VLC_SUCCESS )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
