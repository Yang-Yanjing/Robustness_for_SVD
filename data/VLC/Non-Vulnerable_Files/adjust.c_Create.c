 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        msg_Err( p_filter, "Input and output chromas don't match" );
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_sys = p_filter->p_sys;
    /* needed to get options passed in transcode using the
     * adjust{name=value} syntax */
    config_ChainParse( p_filter, "", ppsz_filter_options,
                   p_filter->p_cfg );
    p_sys->f_contrast = var_CreateGetFloatCommand( p_filter, "contrast" );
    p_sys->f_brightness = var_CreateGetFloatCommand( p_filter, "brightness" );
    p_sys->i_hue = var_CreateGetIntegerCommand( p_filter, "hue" );
    p_sys->f_saturation = var_CreateGetFloatCommand( p_filter, "saturation" );
    p_sys->f_gamma = var_CreateGetFloatCommand( p_filter, "gamma" );
    p_sys->b_brightness_threshold =
        var_CreateGetBoolCommand( p_filter, "brightness-threshold" );
    /* Choose Planar/Packed function and pointer to a Hue/Saturation processing
     * function*/
    switch( p_filter->fmt_in.video.i_chroma )
    {
        CASE_PLANAR_YUV
            /* Planar YUV */
            p_filter->pf_video_filter = FilterPlanar;
            p_sys->pf_process_sat_hue_clip = planar_sat_hue_clip_C;
            p_sys->pf_process_sat_hue = planar_sat_hue_C;
            break;
        CASE_PACKED_YUV_422
            /* Packed YUV 4:2:2 */
            p_filter->pf_video_filter = FilterPacked;
            p_sys->pf_process_sat_hue_clip = packed_sat_hue_clip_C;
            p_sys->pf_process_sat_hue = packed_sat_hue_C;
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                     (char*)&(p_filter->fmt_in.video.i_chroma) );
            return VLC_EGENERIC;
    }
    vlc_mutex_init( &p_sys->lock );
    var_AddCallback( p_filter, "contrast",   AdjustCallback, p_sys );
    var_AddCallback( p_filter, "brightness", AdjustCallback, p_sys );
    var_AddCallback( p_filter, "hue",        AdjustCallback, p_sys );
    var_AddCallback( p_filter, "saturation", AdjustCallback, p_sys );
    var_AddCallback( p_filter, "gamma",      AdjustCallback, p_sys );
    var_AddCallback( p_filter, "brightness-threshold",
                                             AdjustCallback, p_sys );
    return VLC_SUCCESS;
}
