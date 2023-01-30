 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    const vlc_chroma_description_t *p_chroma =
        vlc_fourcc_GetChromaDescription( p_filter->fmt_in.video.i_chroma );
    if( p_chroma == NULL || p_chroma->plane_count == 0 )
        return VLC_EGENERIC;
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_filter->p_sys->p_tmp = picture_NewFromFormat( &p_filter->fmt_in.video );
    if( !p_filter->p_sys->p_tmp )
    {
        free( p_filter->p_sys );
        return VLC_ENOMEM;
    }
    p_filter->p_sys->b_first = true;
    p_filter->pf_video_filter = Filter;
    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    atomic_init( &p_filter->p_sys->i_factor,
             var_CreateGetIntegerCommand( p_filter, FILTER_PREFIX "factor" ) );
    var_AddCallback( p_filter, FILTER_PREFIX "factor",
                     MotionBlurCallback, p_filter->p_sys );
    return VLC_SUCCESS;
}
