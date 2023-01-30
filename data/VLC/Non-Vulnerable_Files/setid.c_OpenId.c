}
static int OpenId( vlc_object_t *p_this )
{
    int i_ret = OpenCommon( p_this );
    if( i_ret != VLC_SUCCESS )
        return i_ret;
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX_ID, ppsz_sout_options_id,
                       p_stream->p_cfg );
    p_stream->p_sys->i_id = var_GetInteger( p_stream, SOUT_CFG_PREFIX_ID "id" );
    p_stream->p_sys->i_new_id = var_GetInteger( p_stream, SOUT_CFG_PREFIX_ID "new-id" );
    p_stream->p_sys->psz_language = NULL;
    p_stream->pf_add = AddId;
    return VLC_SUCCESS;
}
