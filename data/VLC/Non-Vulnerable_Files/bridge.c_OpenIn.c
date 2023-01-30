 *****************************************************************************/
static int OpenIn( vlc_object_t *p_this )
{
    sout_stream_t     *p_stream = (sout_stream_t*)p_this;
    in_sout_stream_sys_t *p_sys;
    vlc_value_t val;
    p_sys          = malloc( sizeof( in_sout_stream_sys_t ) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    if( !p_stream->p_next )
    {
        msg_Err( p_stream, "cannot create chain" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    config_ChainParse( p_stream, SOUT_CFG_PREFIX_IN, ppsz_sout_options_in,
                   p_stream->p_cfg );
    var_Get( p_stream, SOUT_CFG_PREFIX_IN "id-offset", &val );
    p_sys->i_id_offset = val.i_int;
    var_Get( p_stream, SOUT_CFG_PREFIX_IN "delay", &val );
    p_sys->i_delay = (mtime_t)val.i_int * 1000;
    var_Get( p_stream, SOUT_CFG_PREFIX_IN "name", &val );
    if( asprintf( &p_sys->psz_name, "bridge-struct-%s", val.psz_string )<0 )
    {
        free( val.psz_string );
        free( p_sys );
        return VLC_ENOMEM;
    }
    free( val.psz_string );
    var_Get( p_stream, SOUT_CFG_PREFIX_IN "placeholder", &val );
    p_sys->b_placeholder = val.b_bool;
    var_Get( p_stream, SOUT_CFG_PREFIX_IN "placeholder-switch-on-iframe", &val);
    p_sys->b_switch_on_iframe = val.b_bool;
    p_sys->i_state = placeholder_on;
    var_Get( p_stream, SOUT_CFG_PREFIX_IN "placeholder-delay", &val );
    p_sys->i_placeholder_delay = (mtime_t)val.i_int * 1000;
    p_sys->i_last_video = VLC_TS_INVALID;
    p_sys->i_last_audio = VLC_TS_INVALID;
    p_sys->id_video = NULL;
    p_sys->id_audio = NULL;
    p_stream->pf_add    = AddIn;
    p_stream->pf_del    = DelIn;
    p_stream->pf_send   = SendIn;
    p_stream->p_sys     = (sout_stream_sys_t *)p_sys;
    p_stream->pace_nocontrol = true;
    return VLC_SUCCESS;
}
