}
static void dvbsub_pdata4bpp( bs_t *s, uint8_t *p, int i_width, int *pi_off )
{
    bool b_stop = false;
    while( !b_stop && !bs_eof( s ) )
    {
        int i_count = 0, i_color = 0;
        i_color = bs_read( s, 4 );
        if( i_color != 0x00 )
        {
            /* Add 1 pixel */
            i_count = 1;
        }
        else
        {
            if( bs_read( s, 1 ) == 0x00 )           // Switch1
            {
                if( bs_show( s, 3 ) != 0x00 )
                {
                    i_count = 2 + bs_read( s, 3 );
                }
                else
                {
                    bs_skip( s, 3 );
                    b_stop = true;
                }
            }
            else
            {
                if( bs_read( s, 1 ) == 0x00)        //Switch2
                {
                    i_count =  4 + bs_read( s, 2 );
                    i_color = bs_read( s, 4 );
                }
                else
                {
                    switch ( bs_read( s, 2 ) )     //Switch3
                    {
                    case 0x0:
                        i_count = 1;
                        break;
                    case 0x1:
                        i_count = 2;
                        break;
                    case 0x2:
                        i_count = 9 + bs_read( s, 4 );
                        i_color = bs_read( s, 4 );
                        break;
                    case 0x3:
                        i_count= 25 + bs_read( s, 8 );
                        i_color = bs_read( s, 4 );
                        break;
                    }
                }
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
