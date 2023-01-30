*****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    char *psz_method;
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
            p_filter->p_sys->drawingPixelFunction = drawPixelI420;
            COLORS_YUV
            break;
        CASE_PACKED_YUV_422
            p_filter->p_sys->drawingPixelFunction = drawPixelPacked;
            COLORS_YUV
            GetPackedYuvOffsets( p_filter->fmt_in.video.i_chroma,
                                 &p_filter->p_sys->i_y_offset,
                                 &p_filter->p_sys->i_u_offset,
                                 &p_filter->p_sys->i_v_offset );
            break;
        case VLC_CODEC_RGB24:
            p_filter->p_sys->drawingPixelFunction = drawPixelRGB24;
            COLORS_RGB
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                     (char*)&(p_filter->fmt_in.video.i_chroma) );
            return VLC_EGENERIC;
    }
    p_filter->p_sys->p_image = image_HandlerCreate( p_filter );
    if( p_filter->p_sys->p_image == NULL )
        return VLC_EGENERIC;
    p_filter->pf_video_filter = Filter;
    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    if( !(psz_method =
        var_CreateGetNonEmptyStringCommand( p_filter,
                                            FILTER_PREFIX "color" ) ) )
    {
        msg_Err( p_filter, "configuration variable "
                 FILTER_PREFIX "color empty" );
        p_filter->p_sys->ballColor = RED;
    }
    else
        p_filter->p_sys->ballColor = getBallColor( p_this, psz_method );
    free( psz_method );
    p_filter->p_sys->i_ballSize =
            var_CreateGetIntegerCommand( p_filter, FILTER_PREFIX "size" );
    p_filter->p_sys->i_ballSpeed =
            var_CreateGetIntegerCommand( p_filter, FILTER_PREFIX "speed" );
    p_filter->p_sys->b_edgeVisible =
            var_CreateGetBoolCommand( p_filter, FILTER_PREFIX "edge-visible" );
    p_filter->p_sys->i_gradThresh =
            var_CreateGetIntegerCommand( p_filter, FILTER_PREFIX "gradient-threshold" );
    vlc_mutex_init( &p_filter->p_sys->lock );
    var_AddCallback( p_filter, FILTER_PREFIX "color",
                     ballCallback, p_filter->p_sys );
    var_AddCallback( p_filter, FILTER_PREFIX "size",
                     ballCallback, p_filter->p_sys );
    var_AddCallback( p_filter, FILTER_PREFIX "speed",
                     ballCallback, p_filter->p_sys );
    var_AddCallback( p_filter, FILTER_PREFIX "edge-visible",
                     ballCallback, p_filter->p_sys );
    p_filter->p_sys->p_smooth = NULL;
    p_filter->p_sys->p_grad_x = NULL;
    p_filter->p_sys->p_grad_y = NULL;
    p_filter->p_sys->i_ball_x = 100;
    p_filter->p_sys->i_ball_y = 100;
    p_filter->p_sys->f_lastVect_x = 0;
    p_filter->p_sys->f_lastVect_y = -1;
    return VLC_SUCCESS;
}
