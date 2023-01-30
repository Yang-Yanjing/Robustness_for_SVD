 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    if( !p_stream->p_next )
    {
        msg_Err( p_stream, "cannot create chain" );
        return VLC_EGENERIC;
    }
    p_sys = calloc( 1, sizeof( sout_stream_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options,
                   p_stream->p_cfg );
    p_sys->i_id = var_GetInteger( p_stream, SOUT_CFG_PREFIX "id" );
    p_sys->i_delay = 1000 * var_GetInteger( p_stream, SOUT_CFG_PREFIX "delay" );
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->p_sys     = p_sys;
    return VLC_SUCCESS;
}
