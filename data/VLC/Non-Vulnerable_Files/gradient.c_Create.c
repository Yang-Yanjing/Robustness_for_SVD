 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    char *psz_method;
    switch( p_filter->fmt_in.video.i_chroma )
    {
        CASE_PLANAR_YUV
            break;
        default:
             msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                      (char*)&(p_filter->fmt_in.video.i_chroma) );
            return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_filter->pf_video_filter = Filter;
    p_filter->p_sys->p_pre_hough = NULL;
    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                   p_filter->p_cfg );
    if( !(psz_method =
        var_CreateGetNonEmptyStringCommand( p_filter, FILTER_PREFIX "mode" )) )
    {
        msg_Err( p_filter, "configuration variable "
                 FILTER_PREFIX "mode empty" );
        p_filter->p_sys->i_mode = GRADIENT;
    }
    else
    {
        if( !strcmp( psz_method, "gradient" ) )
        {
            p_filter->p_sys->i_mode = GRADIENT;
        }
        else if( !strcmp( psz_method, "edge" ) )
        {
            p_filter->p_sys->i_mode = EDGE;
        }
        else if( !strcmp( psz_method, "hough" ) )
        {
            p_filter->p_sys->i_mode = HOUGH;
        }
        else
        {
            msg_Err( p_filter, "no valid gradient mode provided (%s)", psz_method );
            p_filter->p_sys->i_mode = GRADIENT;
        }
    }
    free( psz_method );
    p_filter->p_sys->i_gradient_type =
        var_CreateGetIntegerCommand( p_filter, FILTER_PREFIX "type" );
    p_filter->p_sys->b_cartoon =
        var_CreateGetBoolCommand( p_filter, FILTER_PREFIX "cartoon" );
    vlc_mutex_init( &p_filter->p_sys->lock );
    var_AddCallback( p_filter, FILTER_PREFIX "mode",
                     GradientCallback, p_filter->p_sys );
    var_AddCallback( p_filter, FILTER_PREFIX "type",
                     GradientCallback, p_filter->p_sys );
    var_AddCallback( p_filter, FILTER_PREFIX "cartoon",
                     GradientCallback, p_filter->p_sys );
    p_filter->p_sys->p_buf32 = NULL;
    p_filter->p_sys->p_buf32_bis = NULL;
    p_filter->p_sys->p_buf8 = NULL;
    return VLC_SUCCESS;
}
