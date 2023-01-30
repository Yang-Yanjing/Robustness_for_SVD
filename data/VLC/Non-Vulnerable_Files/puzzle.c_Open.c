 */
int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    /* Assert video in match with video out */
    if( !es_format_IsSimilar( &p_filter->fmt_in, &p_filter->fmt_out ) ) {
        msg_Err( p_filter, "Input and output format does not match" );
        return VLC_EGENERIC;
    }
    const vlc_chroma_description_t *p_chroma =
        vlc_fourcc_GetChromaDescription( p_filter->fmt_in.video.i_chroma );
    if( p_chroma == NULL || p_chroma->plane_count == 0 )
        return VLC_EGENERIC;
    /* Allocate structure */
    p_filter->p_sys = p_sys = calloc(1, sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* init some values */
    p_sys->b_shuffle_rqst    = true;
    p_sys->b_change_param    = true;
    p_sys->i_mouse_drag_pce  = NO_PCE;
    p_sys->i_pointed_pce     = NO_PCE;
    p_sys->i_magnet_accuracy = 3;
    /* Generate values of random bezier shapes */
    p_sys->ps_bezier_pts_H = calloc( SHAPES_QTY, sizeof( point_t *) );
    if( !p_sys->ps_bezier_pts_H )
    {
        free(p_filter->p_sys);
        p_filter->p_sys = NULL;
        return VLC_ENOMEM;
    }
    for (int32_t i_shape = 0; i_shape<SHAPES_QTY; i_shape++)
        p_sys->ps_bezier_pts_H[i_shape] = puzzle_rand_bezier(7);
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    vlc_mutex_init( &p_sys->lock );
    vlc_mutex_init( &p_sys->pce_lock );
    p_sys->s_new_param.i_rows =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "rows" );
    p_sys->s_new_param.i_cols =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "cols" );
    p_sys->s_new_param.i_border =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "border" );
    p_sys->s_new_param.b_preview =
        var_CreateGetBoolCommand( p_filter, CFG_PREFIX "preview" );
    p_sys->s_new_param.i_preview_size =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "preview-size" );
    p_sys->s_new_param.i_shape_size =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "shape-size" );
    p_sys->s_new_param.i_auto_shuffle_speed =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "auto-shuffle" );
    p_sys->s_new_param.i_auto_solve_speed =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "auto-solve" );
    p_sys->s_new_param.i_rotate =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "rotation" );
    p_sys->s_new_param.i_mode =
        var_CreateGetIntegerCommand( p_filter, CFG_PREFIX "mode" );
    var_AddCallback( p_filter, CFG_PREFIX "rows",         puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "cols",         puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "border",       puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "preview",      puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "preview-size", puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "shape-size",   puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "auto-shuffle", puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "auto-solve",   puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "rotation",     puzzle_Callback, p_sys );
    var_AddCallback( p_filter, CFG_PREFIX "mode",     puzzle_Callback, p_sys );
    p_filter->pf_video_filter = Filter;
    p_filter->pf_video_mouse = puzzle_mouse;
    return VLC_SUCCESS;
}
