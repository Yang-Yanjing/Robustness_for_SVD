}
static void encode_pixel_line_4bp( bs_t *s, subpicture_region_t *p_region,
                                   int i_line )
{
    unsigned int i, i_length = 0;
    int i_pitch = p_region->p_picture->p->i_pitch;
    uint8_t *p_data = &p_region->p_picture->p->p_pixels[ i_pitch * i_line ];
    int i_last_pixel = p_data[0];
    for( i = 0; i <= p_region->fmt.i_visible_width; i++ )
    {
        if( i != p_region->fmt.i_visible_width &&
            p_data[i] == i_last_pixel && i_length != 280 )
        {
            i_length++;
            continue;
        }
        if( ( i_length == 1 ) ||
            ( ( i_length == 3 ) && i_last_pixel ) ||
            ( i_length == 8 ) )
        {
            /* 4bit/pixel code */
            if( i_last_pixel )
                bs_write( s, 4, i_last_pixel );
            else
            {
                bs_write( s, 4, 0 );
                bs_write( s, 1, 1 );
                bs_write( s, 1, 1 );
                bs_write( s, 2, 0 ); /* pseudo color 0 */
            }
            i_length--;
        }
        if( i_length == 2 )
        {
            if( i_last_pixel )
            {
                bs_write( s, 4, i_last_pixel );
                bs_write( s, 4, i_last_pixel );
            }
            else
            {
                bs_write( s, 4, 0 );
                bs_write( s, 1, 1 );
                bs_write( s, 1, 1 );
                bs_write( s, 2, 1 ); /* 2 * pseudo color 0 */
            }
        }
        else if( !i_last_pixel && ( i_length >= 3 ) && ( i_length <= 9 ) )
        {
            bs_write( s, 4, 0 );
            bs_write( s, 1, 0 );
            bs_write( s, 3, i_length - 2 ); /* (i_length - 2) * color 0 */
        }
        else if( i_length > 2 )
        {
            bs_write( s, 4, 0 );
            bs_write( s, 1, 1 );
            if( i_length <= 7 )
            {
                bs_write( s, 1, 0 );
                bs_write( s, 2, i_length - 4 );
                bs_write( s, 4, i_last_pixel );
            }
            else
            {
                bs_write( s, 1, 1 );
                if( i_length <= 24 )
                {
                    bs_write( s, 2, 2 );
                    bs_write( s, 4, i_length - 9 );
                    bs_write( s, 4, i_last_pixel );
                }
                else
                {
                    bs_write( s, 2, 3 );
                    bs_write( s, 8, i_length - 25 );
                    bs_write( s, 4, i_last_pixel );
                }
            }
        }
        if( i == p_region->fmt.i_visible_width ) break;
        i_last_pixel = p_data[i];
        i_length = 1;
    }
    /* Stop */
    bs_write( s, 8, 0 );
    /* Stuffing */
    bs_align_0( s );
}
