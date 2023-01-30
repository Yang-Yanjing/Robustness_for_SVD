}
static void encode_pixel_line_8bp( bs_t *s, subpicture_region_t *p_region,
                                   int i_line )
{
    unsigned int i, i_length = 0;
    int i_pitch = p_region->p_picture->p->i_pitch;
    uint8_t *p_data = &p_region->p_picture->p->p_pixels[ i_pitch * i_line ];
    int i_last_pixel = p_data[0];
    for( i = 0; i <= p_region->fmt.i_visible_width; i++ )
    {
        if( ( i != p_region->fmt.i_visible_width ) &&
            ( p_data[i] == i_last_pixel ) && ( i_length != 127 ) )
        {
            i_length++;
            continue;
        }
        if( ( i_length == 1 ) && i_last_pixel )
        {
            /* 8bit/pixel code */
            bs_write( s, 8, i_last_pixel );
        }
        else if( ( i_length == 2 ) && i_last_pixel )
        {
            /* 8bit/pixel code */
            bs_write( s, 8, i_last_pixel );
            bs_write( s, 8, i_last_pixel );
        }
        else if( i_length <= 127 )
        {
            bs_write( s, 8, 0 );
            if( !i_last_pixel )
            {
                bs_write( s, 1, 0 );
                bs_write( s, 7, i_length ); /* pseudo color 0 */
            }
            else
            {
                bs_write( s, 1, 1 );
                bs_write( s, 7, i_length );
                bs_write( s, 8, i_last_pixel );
            }
        }
        if( i == p_region->fmt.i_visible_width ) break;
        i_last_pixel = p_data[i];
        i_length = 1;
    }
    /* Stop */
    bs_write( s, 8, 0 );
    bs_write( s, 8, 0 );
    /* Stuffing */
    bs_align_0( s );
}
