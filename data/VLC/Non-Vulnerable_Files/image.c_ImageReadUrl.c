}
static picture_t *ImageReadUrl( image_handler_t *p_image, const char *psz_url,
                                video_format_t *p_fmt_in,
                                video_format_t *p_fmt_out )
{
    block_t *p_block;
    picture_t *p_pic;
    stream_t *p_stream = NULL;
    int i_size;
    p_stream = stream_UrlNew( p_image->p_parent, psz_url );
    if( !p_stream )
    {
        msg_Dbg( p_image->p_parent, "could not open %s for reading",
                 psz_url );
        return NULL;
    }
    i_size = stream_Size( p_stream );
    p_block = block_Alloc( i_size );
    stream_Read( p_stream, p_block->p_buffer, i_size );
    if( !p_fmt_in->i_chroma )
    {
        char *psz_mime = NULL;
        stream_Control( p_stream, STREAM_GET_CONTENT_TYPE, &psz_mime );
        if( psz_mime )
            p_fmt_in->i_chroma = image_Mime2Fourcc( psz_mime );
        free( psz_mime );
    }
    stream_Delete( p_stream );
    if( !p_fmt_in->i_chroma )
    {
        /* Try to guess format from file name */
        p_fmt_in->i_chroma = image_Ext2Fourcc( psz_url );
    }
    p_pic = ImageRead( p_image, p_block, p_fmt_in, p_fmt_out );
    return p_pic;
}
