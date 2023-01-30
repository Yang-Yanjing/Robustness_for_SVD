 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys;
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    p_stream->p_sys = p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
        return VLC_ENOMEM;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options, p_stream->p_cfg );
    p_sys->p_out = NULL;
    p_sys->psz_prefix = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "dst-prefix" );
    if( !p_sys->psz_prefix  )
    {
        p_sys->psz_prefix = strdup( "sout-record-" );
        if( !p_sys->psz_prefix )
        {
            free( p_sys );
            return VLC_ENOMEM;
        }
    }
    p_sys->i_date_start = -1;
    p_sys->i_size = 0;
#ifdef OPTIMIZE_MEMORY
    p_sys->i_max_wait = 5*CLOCK_FREQ; /* 5s */
    p_sys->i_max_size = 1*1024*1024; /* 1 MiB */
#else
    p_sys->i_max_wait = 30*CLOCK_FREQ; /* 30s */
    p_sys->i_max_size = 20*1024*1024; /* 20 MiB */
#endif
    p_sys->b_drop = false;
    p_sys->i_dts_start = 0;
    TAB_INIT( p_sys->i_id, p_sys->id );
    return VLC_SUCCESS;
}
