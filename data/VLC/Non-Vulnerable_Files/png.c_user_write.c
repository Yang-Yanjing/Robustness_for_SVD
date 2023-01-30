}
static void user_write( png_structp p_png, png_bytep data, png_size_t i_length )
{
    block_t *p_block = (block_t *)png_get_io_ptr( p_png );
    if( i_length > p_block->i_buffer ) {
        char err_str[64];
        snprintf( err_str, sizeof(err_str),
                  "block size %zu too small for %zu encoded bytes",
                  p_block->i_buffer, i_length );
        png_error( p_png, err_str );
        return;
    }
    memcpy( p_block->p_buffer, data, i_length );
    p_block->p_buffer += i_length;
    p_block->i_buffer -= i_length;
}
