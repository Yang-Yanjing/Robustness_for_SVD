 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    const vlc_fourcc_t fourcc = p_filter->fmt_in.video.i_chroma;
    const vlc_chroma_description_t *p_chroma = vlc_fourcc_GetChromaDescription( fourcc );
    if( !p_chroma || p_chroma->plane_count != 3 || p_chroma->pixel_size != 1 ) {
        msg_Err( p_filter, "Unsupported chroma (%4.4s)", (char*)&fourcc );
        return VLC_EGENERIC;
    }
    /* Allocate structure */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL )
        return VLC_ENOMEM;
    p_filter->pf_video_filter = Filter;
    p_filter->p_sys->x = 10;
    p_filter->p_sys->y = 10;
    p_filter->p_sys->scale = 1;
    p_filter->p_sys->xinc = 1;
    p_filter->p_sys->yinc = 1;
    p_filter->p_sys->scaleinc = 1;
    p_filter->p_sys->u = 0;
    p_filter->p_sys->v = 0;
    p_filter->p_sys->p_image = NULL;
    return VLC_SUCCESS;
}
