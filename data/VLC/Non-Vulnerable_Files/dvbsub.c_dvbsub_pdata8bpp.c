}
static void dvbsub_pdata8bpp( bs_t *s, uint8_t *p, int i_width, int *pi_off )
{
    bool b_stop = false;
    while( !b_stop && !bs_eof( s ) )
    {
        int i_count = 0, i_color = 0;
        i_color = bs_read( s, 8 );
        if( i_color != 0x00 )
        {
            /* Add 1 pixel */
            i_count = 1;
        }
        else
        {
            if( bs_read( s, 1 ) == 0x00 )           // Switch1
            {
                if( bs_show( s, 7 ) != 0x00 )
                {
                    i_count = bs_read( s, 7 );
                }
                else
                {
                    bs_skip( s, 7 );
                    b_stop = true;
                }
            }
            else
            {
                i_count = bs_read( s, 7 );
                i_color = bs_read( s, 8 );
            }
        }
        if( !i_count ) continue;
        /* Sanity check */
        if( ( i_count + *pi_off ) > i_width ) break;
        if( i_count == 1 ) p[*pi_off] = i_color;
        else memset( ( p + *pi_off ), i_color, i_count );
        (*pi_off) += i_count;
    }
    bs_align( s );
}
