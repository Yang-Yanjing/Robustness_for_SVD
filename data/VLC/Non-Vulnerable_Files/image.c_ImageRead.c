 */
static picture_t *ImageRead( image_handler_t *p_image, block_t *p_block,
                             video_format_t *p_fmt_in,
                             video_format_t *p_fmt_out )
{
    picture_t *p_pic = NULL, *p_tmp;
    /* Check if we can reuse the current decoder */
    if( p_image->p_dec &&
        p_image->p_dec->fmt_in.i_codec != p_fmt_in->i_chroma )
    {
        DeleteDecoder( p_image->p_dec );
        p_image->p_dec = 0;
    }
    /* Start a decoder */
    if( !p_image->p_dec )
    {
        p_image->p_dec = CreateDecoder( p_image->p_parent, p_fmt_in );
        if( !p_image->p_dec ) return NULL;
    }
    p_block->i_pts = p_block->i_dts = mdate();
    while( (p_tmp = p_image->p_dec->pf_decode_video( p_image->p_dec, &p_block ))
             != NULL )
    {
        if( p_pic != NULL )
            picture_Release( p_pic );
        p_pic = p_tmp;
    }
    if( p_pic == NULL )
    {
        msg_Warn( p_image->p_parent, "no image decoded" );
        return 0;
    }
    if( !p_fmt_out->i_chroma )
        p_fmt_out->i_chroma = p_image->p_dec->fmt_out.video.i_chroma;
    if( !p_fmt_out->i_width && p_fmt_out->i_height )
        p_fmt_out->i_width = (int64_t)p_image->p_dec->fmt_out.video.i_width *
                             p_image->p_dec->fmt_out.video.i_sar_num *
                             p_fmt_out->i_height /
                             p_image->p_dec->fmt_out.video.i_height /
                             p_image->p_dec->fmt_out.video.i_sar_den;
    if( !p_fmt_out->i_height && p_fmt_out->i_width )
        p_fmt_out->i_height = (int64_t)p_image->p_dec->fmt_out.video.i_height *
                              p_image->p_dec->fmt_out.video.i_sar_den *
                              p_fmt_out->i_width /
                              p_image->p_dec->fmt_out.video.i_width /
                              p_image->p_dec->fmt_out.video.i_sar_num;
    if( !p_fmt_out->i_width )
        p_fmt_out->i_width = p_image->p_dec->fmt_out.video.i_width;
    if( !p_fmt_out->i_height )
        p_fmt_out->i_height = p_image->p_dec->fmt_out.video.i_height;
    if( !p_fmt_out->i_visible_width )
        p_fmt_out->i_visible_width = p_fmt_out->i_width;
    if( !p_fmt_out->i_visible_height )
        p_fmt_out->i_visible_height = p_fmt_out->i_height;
    /* Check if we need chroma conversion or resizing */
    if( p_image->p_dec->fmt_out.video.i_chroma != p_fmt_out->i_chroma ||
        p_image->p_dec->fmt_out.video.i_width != p_fmt_out->i_width ||
        p_image->p_dec->fmt_out.video.i_height != p_fmt_out->i_height )
    {
        if( p_image->p_filter )
        if( p_image->p_filter->fmt_in.video.i_chroma !=
            p_image->p_dec->fmt_out.video.i_chroma ||
            p_image->p_filter->fmt_out.video.i_chroma != p_fmt_out->i_chroma )
        {
            /* We need to restart a new filter */
            DeleteFilter( p_image->p_filter );
            p_image->p_filter = 0;
        }
        /* Start a filter */
        if( !p_image->p_filter )
        {
            p_image->p_filter =
                CreateFilter( p_image->p_parent, &p_image->p_dec->fmt_out,
                              p_fmt_out, NULL );
            if( !p_image->p_filter )
            {
                picture_Release( p_pic );
                return NULL;
            }
        }
        else
        {
            /* Filters should handle on-the-fly size changes */
            p_image->p_filter->fmt_in = p_image->p_dec->fmt_out;
            p_image->p_filter->fmt_out = p_image->p_dec->fmt_out;
            p_image->p_filter->fmt_out.i_codec = p_fmt_out->i_chroma;
            p_image->p_filter->fmt_out.video = *p_fmt_out;
        }
        p_pic = p_image->p_filter->pf_video_filter( p_image->p_filter, p_pic );
        *p_fmt_out = p_image->p_filter->fmt_out.video;
    }
    else *p_fmt_out = p_image->p_dec->fmt_out.video;
    return p_pic;
}
