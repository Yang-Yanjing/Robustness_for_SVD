 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    char *psz_temp, *psz_cmd;
    int i_ret;
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys = p_filter->p_sys;
    p_sys->b_done = false;
    p_filter->pf_video_filter = Filter;
    /* needed to get options passed in transcode using the
     * adjust{name=value} syntax */
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    p_sys->i_loops = var_CreateGetIntegerCommand( p_filter,
                                                  CFG_PREFIX "loops" );
    p_sys->i_alpha = var_CreateGetIntegerCommand( p_filter,
                                                  CFG_PREFIX "alpha" );
    psz_temp = var_CreateGetStringCommand( p_filter, CFG_PREFIX "base-chroma" );
    p_sys->i_base_chroma = VLC_FOURCC( psz_temp[0], psz_temp[1],
                                       psz_temp[2], psz_temp[3] );
    psz_cmd = var_CreateGetStringCommand( p_filter, CFG_PREFIX "base-image" );
    i_ret = blendbench_LoadImage( p_this, &p_sys->p_base_image,
                                  p_sys->i_base_chroma, psz_cmd, "Base" );
    free( psz_temp );
    free( psz_cmd );
    if( i_ret != VLC_SUCCESS )
    {
        free( p_sys );
        return i_ret;
    }
    psz_temp = var_CreateGetStringCommand( p_filter,
                                           CFG_PREFIX "blend-chroma" );
    p_sys->i_blend_chroma = VLC_FOURCC( psz_temp[0], psz_temp[1],
                                        psz_temp[2], psz_temp[3] );
    psz_cmd = var_CreateGetStringCommand( p_filter, CFG_PREFIX "blend-image" );
    blendbench_LoadImage( p_this, &p_sys->p_blend_image, p_sys->i_blend_chroma,
                          psz_cmd, "Blend" );
    free( psz_temp );
    free( psz_cmd );
    return VLC_SUCCESS;
}
