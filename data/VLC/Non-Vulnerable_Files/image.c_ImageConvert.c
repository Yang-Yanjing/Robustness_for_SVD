 */
static picture_t *ImageConvert( image_handler_t *p_image, picture_t *p_pic,
                                video_format_t *p_fmt_in,
                                video_format_t *p_fmt_out )
{
    picture_t *p_pif;
    if( !p_fmt_out->i_width && !p_fmt_out->i_height &&
        p_fmt_out->i_sar_num && p_fmt_out->i_sar_den &&
        p_fmt_out->i_sar_num * p_fmt_in->i_sar_den !=
        p_fmt_out->i_sar_den * p_fmt_in->i_sar_num )
    {
        p_fmt_out->i_width =
            p_fmt_in->i_sar_num * (int64_t)p_fmt_out->i_sar_den *
            p_fmt_in->i_width / p_fmt_in->i_sar_den / p_fmt_out->i_sar_num;
        p_fmt_out->i_visible_width =
            p_fmt_in->i_sar_num * (int64_t)p_fmt_out->i_sar_den *
            p_fmt_in->i_visible_width / p_fmt_in->i_sar_den /
            p_fmt_out->i_sar_num;
    }
    if( !p_fmt_out->i_chroma ) p_fmt_out->i_chroma = p_fmt_in->i_chroma;
    if( !p_fmt_out->i_width )
        p_fmt_out->i_width = p_fmt_out->i_visible_width = p_fmt_in->i_width;
    if( !p_fmt_out->i_height )
        p_fmt_out->i_height = p_fmt_out->i_visible_height = p_fmt_in->i_height;
    if( !p_fmt_out->i_sar_num ) p_fmt_out->i_sar_num = p_fmt_in->i_sar_num;
    if( !p_fmt_out->i_sar_den ) p_fmt_out->i_sar_den = p_fmt_in->i_sar_den;
    if( p_image->p_filter )
    if( p_image->p_filter->fmt_in.video.i_chroma != p_fmt_in->i_chroma ||
        p_image->p_filter->fmt_out.video.i_chroma != p_fmt_out->i_chroma )
    {
        /* We need to restart a new filter */
        DeleteFilter( p_image->p_filter );
        p_image->p_filter = NULL;
    }
    /* Start a filter */
    if( !p_image->p_filter )
    {
        es_format_t fmt_in;
        es_format_Init( &fmt_in, VIDEO_ES, p_fmt_in->i_chroma );
        fmt_in.video = *p_fmt_in;
        p_image->p_filter =
            CreateFilter( p_image->p_parent, &fmt_in, p_fmt_out, NULL );
        if( !p_image->p_filter )
        {
            return NULL;
        }
    }
    else
    {
        /* Filters should handle on-the-fly size changes */
        p_image->p_filter->fmt_in.video = *p_fmt_in;
        p_image->p_filter->fmt_out.video = *p_fmt_out;
    }
    picture_Hold( p_pic );
    p_pif = p_image->p_filter->pf_video_filter( p_image->p_filter, p_pic );
    if( p_fmt_in->i_chroma == p_fmt_out->i_chroma &&
        p_fmt_in->i_width == p_fmt_out->i_width &&
        p_fmt_in->i_height == p_fmt_out->i_height )
    {
        /* Duplicate image */
        picture_Release( p_pif ); /* XXX: Better fix must be possible */
        p_pif = p_image->p_filter->pf_video_buffer_new( p_image->p_filter );
        if( p_pif )
            picture_Copy( p_pif, p_pic );
    }
    return p_pif;
}
