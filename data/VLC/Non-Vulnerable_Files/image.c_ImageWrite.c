 */
static block_t *ImageWrite( image_handler_t *p_image, picture_t *p_pic,
                            video_format_t *p_fmt_in,
                            video_format_t *p_fmt_out )
{
    block_t *p_block;
    /* Check if we can reuse the current encoder */
    if( p_image->p_enc &&
        ( p_image->p_enc->fmt_out.i_codec != p_fmt_out->i_chroma ||
          p_image->p_enc->fmt_out.video.i_width != p_fmt_out->i_width ||
          p_image->p_enc->fmt_out.video.i_height != p_fmt_out->i_height ) )
    {
        DeleteEncoder( p_image->p_enc );
        p_image->p_enc = 0;
    }
    /* Start an encoder */
    if( !p_image->p_enc )
    {
        p_image->p_enc = CreateEncoder( p_image->p_parent,
                                        p_fmt_in, p_fmt_out );
        if( !p_image->p_enc ) return NULL;
    }
    /* Check if we need chroma conversion or resizing */
    if( p_image->p_enc->fmt_in.video.i_chroma != p_fmt_in->i_chroma ||
        p_image->p_enc->fmt_in.video.i_width != p_fmt_in->i_width ||
        p_image->p_enc->fmt_in.video.i_height != p_fmt_in->i_height )
    {
        picture_t *p_tmp_pic;
        if( p_image->p_filter )
        if( p_image->p_filter->fmt_in.video.i_chroma != p_fmt_in->i_chroma ||
            p_image->p_filter->fmt_out.video.i_chroma !=
            p_image->p_enc->fmt_in.video.i_chroma )
        {
            /* We need to restart a new filter */
            DeleteFilter( p_image->p_filter );
            p_image->p_filter = 0;
        }
        /* Start a filter */
        if( !p_image->p_filter )
        {
            es_format_t fmt_in;
            es_format_Init( &fmt_in, VIDEO_ES, p_fmt_in->i_chroma );
            fmt_in.video = *p_fmt_in;
            p_image->p_filter =
                CreateFilter( p_image->p_parent, &fmt_in,
                              &p_image->p_enc->fmt_in.video, NULL );
            if( !p_image->p_filter )
            {
                return NULL;
            }
        }
        else
        {
            /* Filters should handle on-the-fly size changes */
            p_image->p_filter->fmt_in.i_codec = p_fmt_in->i_chroma;
            p_image->p_filter->fmt_out.video = *p_fmt_in;
            p_image->p_filter->fmt_out.i_codec =p_image->p_enc->fmt_in.i_codec;
            p_image->p_filter->fmt_out.video = p_image->p_enc->fmt_in.video;
        }
        picture_Hold( p_pic );
        p_tmp_pic =
            p_image->p_filter->pf_video_filter( p_image->p_filter, p_pic );
        if( likely(p_tmp_pic != NULL) )
        {
            p_block = p_image->p_enc->pf_encode_video( p_image->p_enc,
                                                       p_tmp_pic );
            p_image->p_filter->pf_video_buffer_del( p_image->p_filter,
                                                    p_tmp_pic );
        }
        else
            p_block = NULL;
    }
    else
    {
        p_block = p_image->p_enc->pf_encode_video( p_image->p_enc, p_pic );
    }
    if( !p_block )
    {
        msg_Dbg( p_image->p_parent, "no image encoded" );
        return 0;
    }
    return p_block;
}
