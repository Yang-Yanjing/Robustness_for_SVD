 */
static picture_t *ImageFilter( image_handler_t *p_image, picture_t *p_pic,
                               video_format_t *p_fmt, const char *psz_module )
{
    /* Start a filter */
    if( !p_image->p_filter )
    {
        es_format_t fmt;
        es_format_Init( &fmt, VIDEO_ES, p_fmt->i_chroma );
        fmt.video = *p_fmt;
        p_image->p_filter =
            CreateFilter( p_image->p_parent, &fmt, &fmt.video, psz_module );
        if( !p_image->p_filter )
        {
            return NULL;
        }
    }
    else
    {
        /* Filters should handle on-the-fly size changes */
        p_image->p_filter->fmt_in.video = *p_fmt;
        p_image->p_filter->fmt_out.video = *p_fmt;
    }
    picture_Hold( p_pic );
    return p_image->p_filter->pf_video_filter( p_image->p_filter, p_pic );
}
