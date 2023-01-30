}
static void ClosePlayItem( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if( p_sys->p_m2ts )
        stream_Delete( p_sys->p_m2ts );
    if( p_sys->p_parser )
        stream_Delete( p_sys->p_parser );
    es_out_Control( p_demux->out, ES_OUT_RESET_PCR );
}
