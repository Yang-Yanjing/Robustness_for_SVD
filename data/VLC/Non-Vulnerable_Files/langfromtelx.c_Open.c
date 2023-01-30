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
    p_sys = malloc( sizeof( sout_stream_sys_t ) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options,
                       p_stream->p_cfg );
    p_sys->i_id       = var_GetInteger( p_stream, SOUT_CFG_PREFIX "id" );
    p_sys->i_magazine = var_GetInteger( p_stream, SOUT_CFG_PREFIX "magazine" );
    p_sys->i_page     = var_GetInteger( p_stream, SOUT_CFG_PREFIX "page" );
    p_sys->i_row      = var_GetInteger( p_stream, SOUT_CFG_PREFIX "row" );
    p_stream->pf_add  = Add;
    p_stream->pf_del  = Del;
    p_stream->pf_send = Send;
    p_stream->p_sys   = p_sys;
    return VLC_SUCCESS;
}
