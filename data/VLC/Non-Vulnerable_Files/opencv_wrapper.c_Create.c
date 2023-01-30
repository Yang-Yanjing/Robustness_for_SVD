 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t* p_filter = (filter_t*)p_this;
    char *psz_chroma, *psz_output;
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    /* Load the internal OpenCV filter.
     *
     * This filter object is needed to call the internal OpenCV filter
     * for processing, the wrapper just converts into an IplImage* for
     * the other filter.
     *
     * We don't need to set up video formats for this filter as it not
     * actually using a picture_t.
     */
    p_filter->p_sys->p_opencv = vlc_object_create( p_filter, sizeof(filter_t) );
    if( !p_filter->p_sys->p_opencv ) {
        free( p_filter->p_sys );
        return VLC_ENOMEM;
    }
    p_filter->p_sys->psz_inner_name = var_InheritString( p_filter, "opencv-filter-name" );
    if( p_filter->p_sys->psz_inner_name )
        p_filter->p_sys->p_opencv->p_module =
            module_need( p_filter->p_sys->p_opencv,
                         "opencv internal filter",
                         p_filter->p_sys->psz_inner_name,
                         true );
    if( !p_filter->p_sys->p_opencv->p_module )
    {
        msg_Err( p_filter, "can't open internal opencv filter: %s", p_filter->p_sys->psz_inner_name );
        free( p_filter->p_sys->psz_inner_name );
        p_filter->p_sys->psz_inner_name = NULL;
        vlc_object_release( p_filter->p_sys->p_opencv );
        free( p_filter->p_sys );
        return VLC_ENOMOD;
    }
    /* Init structure */
    p_filter->p_sys->p_image = image_HandlerCreate( p_filter );
    for( int i = 0; i < VOUT_MAX_PLANES; i++ )
        p_filter->p_sys->p_cv_image[i] = NULL;
    p_filter->p_sys->p_proc_image = NULL;
    p_filter->p_sys->p_to_be_freed = NULL;
    p_filter->p_sys->i_cv_image_size = 0;
    /* Retrieve and apply config */
    psz_chroma = var_InheritString( p_filter, "opencv-chroma" );
    if( psz_chroma == NULL )
    {
        msg_Err( p_filter, "configuration variable %s empty, using 'grey'",
                         "opencv-chroma" );
        p_filter->p_sys->i_internal_chroma = GREY;
    } else if( !strcmp( psz_chroma, "input" ) )
        p_filter->p_sys->i_internal_chroma = CINPUT;
    else if( !strcmp( psz_chroma, "I420" ) )
        p_filter->p_sys->i_internal_chroma = GREY;
    else if( !strcmp( psz_chroma, "RGB32" ) )
        p_filter->p_sys->i_internal_chroma = RGB;
    else {
        msg_Err( p_filter, "no valid opencv-chroma provided, using 'grey'" );
        p_filter->p_sys->i_internal_chroma = GREY;
    }
    free( psz_chroma );
    psz_output = var_InheritString( p_filter, "opencv-output" );
    if( psz_output == NULL )
    {
        msg_Err( p_filter, "configuration variable %s empty, using 'input'",
                         "opencv-output" );
        p_filter->p_sys->i_wrapper_output = VINPUT;
    } else if( !strcmp( psz_output, "none" ) )
        p_filter->p_sys->i_wrapper_output = NONE;
    else if( !strcmp( psz_output, "input" ) )
        p_filter->p_sys->i_wrapper_output = VINPUT;
    else if( !strcmp( psz_output, "processed" ) )
        p_filter->p_sys->i_wrapper_output = PROCESSED;
    else {
        msg_Err( p_filter, "no valid opencv-output provided, using 'input'" );
        p_filter->p_sys->i_wrapper_output = VINPUT;
    }
    free( psz_output );
    p_filter->p_sys->f_scale =
        var_InheritFloat( p_filter, "opencv-scale" );
    msg_Info(p_filter, "Configuration: opencv-scale: %f, opencv-chroma: %d, "
        "opencv-output: %d, opencv-filter %s",
        p_filter->p_sys->f_scale,
        p_filter->p_sys->i_internal_chroma,
        p_filter->p_sys->i_wrapper_output,
        p_filter->p_sys->psz_inner_name);
#ifndef NDEBUG
    msg_Dbg( p_filter, "opencv_wrapper successfully started" );
#endif
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
