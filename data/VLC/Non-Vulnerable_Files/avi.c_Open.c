 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_mux_t      *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t  *p_sys;
    msg_Dbg( p_mux, "AVI muxer opened" );
    p_sys = malloc( sizeof( sout_mux_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_streams = 0;
    p_sys->i_stream_video = -1;
    p_sys->i_movi_size = 0;
    p_sys->idx1.i_entry_count = 0;
    p_sys->idx1.i_entry_max = 10000;
    p_sys->idx1.entry = calloc( p_sys->idx1.i_entry_max,
                                sizeof( avi_idx1_entry_t ) );
    if( !p_sys->idx1.entry )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    p_sys->b_write_header = true;
    p_mux->pf_control   = Control;
    p_mux->pf_addstream = AddStream;
    p_mux->pf_delstream = DelStream;
    p_mux->pf_mux       = Mux;
    p_mux->p_sys        = p_sys;
    return VLC_SUCCESS;
}
