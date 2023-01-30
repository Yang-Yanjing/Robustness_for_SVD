}
static void user_read( png_structp p_png, png_bytep data, png_size_t i_length )
{
    block_t *p_block = (block_t *)png_get_io_ptr( p_png );
    if( i_length > p_block->i_buffer ) {
        png_error( p_png, "not enough data" );
        return;
    }
    memcpy( data, p_block->p_buffer, i_length );
    p_block->p_buffer += i_length;
    p_block->i_buffer -= i_length;
}
