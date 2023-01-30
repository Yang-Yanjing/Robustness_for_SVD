 */
static int ParseVOL( decoder_t *p_dec, es_format_t *fmt,
                     uint8_t *p_vol, int i_vol )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int i_vo_type, i_vo_ver_id, i_ar, i_shape;
    bs_t s;
    for( ;; )
    {
        if( p_vol[0] == 0x00 && p_vol[1] == 0x00 && p_vol[2] == 0x01 &&
            p_vol[3] >= 0x20 && p_vol[3] <= 0x2f ) break;
        p_vol++; i_vol--;
        if( i_vol <= 4 ) return VLC_EGENERIC;
    }
    bs_init( &s, &p_vol[4], i_vol - 4 );
    bs_skip( &s, 1 );   /* random access */
    i_vo_type = bs_read( &s, 8 );
    if( bs_read1( &s ) )
    {
        i_vo_ver_id = bs_read( &s, 4 );
        bs_skip( &s, 3 );
    }
    else
    {
        i_vo_ver_id = 1;
    }
    i_ar = bs_read( &s, 4 );
    if( i_ar == 0xf )
    {
        int i_ar_width, i_ar_height;
        i_ar_width = bs_read( &s, 8 );
        i_ar_height= bs_read( &s, 8 );
    }
    if( bs_read1( &s ) )
    {
        int i_chroma_format;
        int i_low_delay;
        /* vol control parameter */
        i_chroma_format = bs_read( &s, 2 );
        i_low_delay = bs_read1( &s );
        if( bs_read1( &s ) )
        {
            bs_skip( &s, 16 );
            bs_skip( &s, 16 );
            bs_skip( &s, 16 );
            bs_skip( &s, 3 );
            bs_skip( &s, 11 );
            bs_skip( &s, 1 );
            bs_skip( &s, 16 );
        }
    }
    /* shape 0->RECT, 1->BIN, 2->BIN_ONLY, 3->GRAY */
    i_shape = bs_read( &s, 2 );
    if( i_shape == 3 && i_vo_ver_id != 1 )
    {
        bs_skip( &s, 4 );
    }
    if( !bs_read1( &s ) ) return VLC_EGENERIC; /* Marker */
    p_sys->i_fps_num = bs_read( &s, 16 ); /* Time increment resolution*/
    if( !p_sys->i_fps_num ) p_sys->i_fps_num = 1;
    if( !bs_read1( &s ) ) return VLC_EGENERIC; /* Marker */
    if( bs_read1( &s ) )
    {
        int i_time_increment_bits = vlc_log2( p_sys->i_fps_num - 1 ) + 1;
        if( i_time_increment_bits < 1 ) i_time_increment_bits = 1;
        p_sys->i_fps_den = bs_read( &s, i_time_increment_bits );
    }
    if( i_shape == 0 )
    {
        bs_skip( &s, 1 );
        fmt->video.i_width = bs_read( &s, 13 );
        bs_skip( &s, 1 );
        fmt->video.i_height= bs_read( &s, 13 );
        bs_skip( &s, 1 );
    }
    return VLC_SUCCESS;
}
