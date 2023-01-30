 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
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
    p_filter->p_sys = malloc( sizeof( *p_filter->p_sys ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_filter->pf_video_filter = Filter;
    /* Initialize the arguments */
    atomic_init( &p_filter->p_sys->i_window_size,
                var_CreateGetIntegerCommand( p_filter,
                                             FILTER_PREFIX"window-size" ) );
    atomic_init( &p_filter->p_sys->i_softening,
                 var_CreateGetIntegerCommand( p_filter,
                                             FILTER_PREFIX"softening-size" ) );
    p_filter->p_sys->p_old_data = calloc( p_filter->fmt_in.video.i_width *
     (p_filter->fmt_in.video.i_height+1),sizeof(*p_filter->p_sys->p_old_data) );
    if( p_filter->p_sys->p_old_data == NULL )
    {
        free( p_filter->p_sys );
        return VLC_ENOMEM;
    }
    memset( p_filter->p_sys->ia_luminance_data, 0,
                    sizeof(p_filter->p_sys->ia_luminance_data) );
    p_filter->p_sys->ia_luminance_data[p_filter->p_sys->i_window_size - 1] = 256;
    var_AddCallback(p_filter,FILTER_PREFIX "window-size",
        AntiFlickerCallback, p_filter->p_sys);
    var_AddCallback(p_filter,FILTER_PREFIX "softening-size",
        AntiFlickerCallback, p_filter->p_sys);
    return VLC_SUCCESS;
}
