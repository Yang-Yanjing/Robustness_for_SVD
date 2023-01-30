 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* Allocate structure */
    p_sys = p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->pf_sepia = NULL;
    for( int i = 0; p_sepia_cfg[i].i_chroma != 0; i++ )
    {
        if( p_sepia_cfg[i].i_chroma != p_filter->fmt_in.video.i_chroma )
            continue;
        p_sys->pf_sepia = p_sepia_cfg[i].pf_sepia;
    }
    if( p_sys->pf_sepia == NULL )
    {
        msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                (char*)&(p_filter->fmt_in.video.i_chroma) );
        free( p_sys );
        return VLC_EGENERIC;
    }
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    atomic_init( &p_sys->i_intensity,
             var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "intensity" ) );
    var_AddCallback( p_filter, CFG_PREFIX "intensity", FilterCallback, NULL );
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
