 *****************************************************************************/
static void RenderImage( decoder_t *p_dec, block_t *p_data,
                         subpicture_region_t *p_region )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t *p_dest = p_region->p_picture->Y_PIXELS;
    int i_field;            /* The subtitles are interlaced */
    int i_row, i_column;    /* scanline row/column number */
    uint8_t i_color, i_count;
    bs_t bs;
    bs_init( &bs, p_data->p_buffer + p_sys->i_image_offset,
             p_data->i_buffer - p_sys->i_image_offset );
    for( i_field = 0; i_field < 2; i_field++ )
    {
        for( i_row = i_field; i_row < p_sys->i_height; i_row += 2 )
        {
            for( i_column = 0; i_column < p_sys->i_width; i_column++ )
            {
                uint8_t i_val = bs_read( &bs, 4 );
                if( i_val == 0 )
                {
                    /* Fill the rest of the line with next color */
                    i_color = bs_read( &bs, 4 );
                    memset( &p_dest[i_row * p_region->p_picture->Y_PITCH +
                                    i_column], i_color,
                            p_sys->i_width - i_column );
                    i_column = p_sys->i_width;
                    continue;
                }
                else
                {
                    /* Normal case: get color and repeat count */
                    i_count = (i_val >> 2);
                    i_color = i_val & 0x3;
                    i_count = __MIN( i_count, p_sys->i_width - i_column );
                    memset( &p_dest[i_row * p_region->p_picture->Y_PITCH +
                                    i_column], i_color, i_count );
                    i_column += i_count - 1;
                    continue;
                }
            }
            bs_align( &bs );
        }
    }
}
