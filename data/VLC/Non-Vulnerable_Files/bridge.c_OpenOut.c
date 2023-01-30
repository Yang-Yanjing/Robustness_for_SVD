 *****************************************************************************/
static int OpenOut( vlc_object_t *p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t *)p_this;
    out_sout_stream_sys_t *p_sys;
    vlc_value_t val;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX_OUT, ppsz_sout_options_out,
                   p_stream->p_cfg );
    p_sys          = malloc( sizeof( out_sout_stream_sys_t ) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    p_sys->b_inited = false;
    var_Get( p_stream, SOUT_CFG_PREFIX_OUT "id", &val );
    p_sys->i_id = val.i_int;
    var_Get( p_stream, SOUT_CFG_PREFIX_OUT "in-name", &val );
    if( asprintf( &p_sys->psz_name, "bridge-struct-%s", val.psz_string )<0 )
    {
        free( val.psz_string );
        free( p_sys );
        return VLC_ENOMEM;
    }
    free( val.psz_string );
    p_stream->pf_add    = AddOut;
    p_stream->pf_del    = DelOut;
    p_stream->pf_send   = SendOut;
    p_stream->p_sys     = (sout_stream_sys_t *)p_sys;
    p_stream->pace_nocontrol = true;
    return VLC_SUCCESS;
}
