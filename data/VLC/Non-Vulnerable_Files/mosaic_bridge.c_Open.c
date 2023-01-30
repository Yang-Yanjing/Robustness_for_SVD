 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t        *p_stream = (sout_stream_t *)p_this;
    sout_stream_sys_t    *p_sys;
    vlc_value_t           val;
    config_ChainParse( p_stream, CFG_PREFIX, ppsz_sout_options,
                       p_stream->p_cfg );
    p_sys = malloc( sizeof( sout_stream_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_stream->p_sys = p_sys;
    p_sys->b_inited = false;
    p_sys->psz_id = var_CreateGetString( p_stream, CFG_PREFIX "id" );
    p_sys->i_height =
        var_CreateGetIntegerCommand( p_stream, CFG_PREFIX "height" );
    var_AddCallback( p_stream, CFG_PREFIX "height", HeightCallback, p_stream );
    p_sys->i_width =
        var_CreateGetIntegerCommand( p_stream, CFG_PREFIX "width" );
    var_AddCallback( p_stream, CFG_PREFIX "width", WidthCallback, p_stream );
    var_Get( p_stream, CFG_PREFIX "sar", &val );
    if( val.psz_string )
    {
        char *psz_parser = strchr( val.psz_string, ':' );
        if( psz_parser )
        {
            *psz_parser++ = '\0';
            p_sys->i_sar_num = atoi( val.psz_string );
            p_sys->i_sar_den = atoi( psz_parser );
            vlc_ureduce( &p_sys->i_sar_num, &p_sys->i_sar_den,
                         p_sys->i_sar_num, p_sys->i_sar_den, 0 );
        }
        else
        {
            msg_Warn( p_stream, "bad aspect ratio %s", val.psz_string );
            p_sys->i_sar_num = p_sys->i_sar_den = 1;
        }
        free( val.psz_string );
    }
    else
    {
        p_sys->i_sar_num = p_sys->i_sar_den = 1;
    }
    p_sys->i_chroma = 0;
    val.psz_string = var_GetNonEmptyString( p_stream, CFG_PREFIX "chroma" );
    if( val.psz_string && strlen( val.psz_string ) >= 4 )
    {
        memcpy( &p_sys->i_chroma, val.psz_string, 4 );
        msg_Dbg( p_stream, "Forcing image chroma to 0x%.8x (%4.4s)", p_sys->i_chroma, (char*)&p_sys->i_chroma );
    }
    free( val.psz_string );
#define INT_COMMAND( a ) do { \
    var_Create( p_stream, CFG_PREFIX #a, \
                VLC_VAR_INTEGER | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND ); \
    var_AddCallback( p_stream, CFG_PREFIX #a, a ## Callback, \
                     p_stream ); } while(0)
    INT_COMMAND( alpha );
    INT_COMMAND( x );
    INT_COMMAND( y );
#undef INT_COMMAND
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->pace_nocontrol = true;
    return VLC_SUCCESS;
}
