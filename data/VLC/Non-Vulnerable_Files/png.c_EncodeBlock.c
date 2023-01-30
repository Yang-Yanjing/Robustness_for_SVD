 */
static block_t *EncodeBlock(encoder_t *p_enc, picture_t *p_pic)
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    if( unlikely( !p_pic ) )
    {
        return NULL;
    }
    block_t *p_block = block_Alloc( p_sys->i_blocksize );
    if( p_block == NULL )
    {
        return NULL;
    }
    png_structp p_png = png_create_write_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
    if( p_png == NULL )
    {
        block_Release( p_block );
        return NULL;
    }
    /* Disable filtering to speed-up encoding */
    png_set_filter( p_png, 0, PNG_NO_FILTERS );
    /* 1 == best speed */
    png_set_compression_level( p_png, 1 );
    /* save buffer start */
    uint8_t *p_start = p_block->p_buffer;
    size_t i_start = p_block->i_buffer;
    p_sys->b_error = false;
    png_infop p_info = NULL;
    /* libpng longjmp's there in case of error */
    if( setjmp( png_jmpbuf( p_png ) ) )
        goto error;
    png_set_write_fn( p_png, p_block, user_write, user_flush );
    png_set_error_fn( p_png, p_enc, user_error, user_warning );
    p_info = png_create_info_struct( p_png );
    if( p_info == NULL )
        goto error;
    png_infop p_end_info = png_create_info_struct( p_png );
    if( p_end_info == NULL ) goto error;
    png_set_IHDR( p_png, p_info,
            p_enc->fmt_in.video.i_visible_width,
            p_enc->fmt_in.video.i_visible_height,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
    if( p_sys->b_error ) goto error;
    png_write_info( p_png, p_info );
    if( p_sys->b_error ) goto error;
    /* Encode picture */
    for( int i = 0; i < p_pic->p->i_visible_lines; i++ )
    {
        png_write_row( p_png, p_pic->p->p_pixels + (i * p_pic->p->i_pitch) );
        if( p_sys->b_error ) goto error;
    }
    png_write_end( p_png, p_end_info );
    if( p_sys->b_error ) goto error;
    png_destroy_write_struct( &p_png, &p_info );
    /* restore original buffer position */
    p_block->p_buffer = p_start;
    p_block->i_buffer = i_start - p_block->i_buffer;
    p_block->i_dts = p_block->i_pts = p_pic->date;
    return p_block;
 error:
    png_destroy_write_struct( &p_png, p_info ? &p_info : NULL );
    block_Release(p_block);
    return NULL;
}
