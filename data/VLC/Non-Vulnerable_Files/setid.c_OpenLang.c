}
static int OpenLang( vlc_object_t *p_this )
{
    int i_ret = OpenCommon( p_this );
    if( i_ret != VLC_SUCCESS )
        return i_ret;
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX_LANG, ppsz_sout_options_lang,
                       p_stream->p_cfg );
    p_stream->p_sys->i_id = var_GetInteger( p_stream, SOUT_CFG_PREFIX_LANG "id" );
    p_stream->p_sys->i_new_id = -1;
    p_stream->p_sys->psz_language = var_GetString( p_stream, SOUT_CFG_PREFIX_LANG "lang" );
    p_stream->pf_add = AddLang;
    return VLC_SUCCESS;
}
