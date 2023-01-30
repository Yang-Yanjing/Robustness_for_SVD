}
static int ImageWriteUrl( image_handler_t *p_image, picture_t *p_pic,
                          video_format_t *p_fmt_in, video_format_t *p_fmt_out,
                          const char *psz_url )
{
    block_t *p_block;
    FILE *file;
    if( !p_fmt_out->i_chroma )
    {
        /* Try to guess format from file name */
        p_fmt_out->i_chroma = image_Ext2Fourcc( psz_url );
    }
    file = vlc_fopen( psz_url, "wb" );
    if( !file )
    {
        msg_Err( p_image->p_parent, "%s: %s", psz_url, vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    p_block = ImageWrite( p_image, p_pic, p_fmt_in, p_fmt_out );
    int err = 0;
    if( p_block )
    {
        if( fwrite( p_block->p_buffer, p_block->i_buffer, 1, file ) != 1 )
            err = errno;
        block_Release( p_block );
    }
    if( fclose( file ) && !err )
        err = errno;
    if( err )
    {
       errno = err;
       msg_Err( p_image->p_parent, "%s: %s", psz_url, vlc_strerror_c(errno) );
    }
    return err ? VLC_EGENERIC : VLC_SUCCESS;
}
