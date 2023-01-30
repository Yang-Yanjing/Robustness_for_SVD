}
int GetSyncInfo( const uint8_t *p_buf,
                        bool *pb_dts_hd,
                        unsigned int *pi_sample_rate,
                        unsigned int *pi_bit_rate,
                        unsigned int *pi_frame_length,
                        unsigned int *pi_audio_mode )
{
    unsigned int i_frame_size;
    /* 14 bits, little endian version of the bitstream */
    if( p_buf[0] == 0xff && p_buf[1] == 0x1f &&
        p_buf[2] == 0x00 && p_buf[3] == 0xe8 &&
        (p_buf[4] & 0xf0) == 0xf0 && p_buf[5] == 0x07 )
    {
        uint8_t conv_buf[DTS_HEADER_SIZE];
        Buf14To16( conv_buf, p_buf, DTS_HEADER_SIZE, 1 );
        i_frame_size = SyncInfo16be( conv_buf, pi_audio_mode, pi_sample_rate,
                                     pi_bit_rate, pi_frame_length );
        i_frame_size = i_frame_size * 8 / 14 * 2;
    }
    /* 14 bits, big endian version of the bitstream */
    else if( p_buf[0] == 0x1f && p_buf[1] == 0xff &&
             p_buf[2] == 0xe8 && p_buf[3] == 0x00 &&
             p_buf[4] == 0x07 && (p_buf[5] & 0xf0) == 0xf0 )
    {
        uint8_t conv_buf[DTS_HEADER_SIZE];
        Buf14To16( conv_buf, p_buf, DTS_HEADER_SIZE, 0 );
        i_frame_size = SyncInfo16be( conv_buf, pi_audio_mode, pi_sample_rate,
                                     pi_bit_rate, pi_frame_length );
        i_frame_size = i_frame_size * 8 / 14 * 2;
    }
    /* 16 bits, big endian version of the bitstream */
    else if( p_buf[0] == 0x7f && p_buf[1] == 0xfe &&
             p_buf[2] == 0x80 && p_buf[3] == 0x01 )
    {
        i_frame_size = SyncInfo16be( p_buf, pi_audio_mode, pi_sample_rate,
                                     pi_bit_rate, pi_frame_length );
    }
    /* 16 bits, little endian version of the bitstream */
    else if( p_buf[0] == 0xfe && p_buf[1] == 0x7f &&
             p_buf[2] == 0x01 && p_buf[3] == 0x80 )
    {
        uint8_t conv_buf[DTS_HEADER_SIZE];
        BufLeToBe( conv_buf, p_buf, DTS_HEADER_SIZE );
        i_frame_size = SyncInfo16be( p_buf, pi_audio_mode, pi_sample_rate,
                                     pi_bit_rate, pi_frame_length );
    }
    /* DTS-HD */
    else if( p_buf[0] == 0x64 && p_buf[1] ==  0x58 &&
                p_buf[2] == 0x20 && p_buf[3] ==  0x25 )
    {
        int i_dts_hd_size;
        bs_t s;
        bs_init( &s, &p_buf[4], DTS_HEADER_SIZE - 4 );
        bs_skip( &s, 8 + 2 );
        if( bs_read1( &s ) )
        {
            bs_skip( &s, 12 );
            i_dts_hd_size = bs_read( &s, 20 ) + 1;
        }
        else
        {
            bs_skip( &s, 8 );
            i_dts_hd_size = bs_read( &s, 16 ) + 1;
        }
        //uint16_t s0 = bs_read( &s, 16 );
        //uint16_t s1 = bs_read( &s, 16 );
        //fprintf( stderr, "DTS HD=%d : %x %x\n", i_dts_hd_size, s0, s1 );
        *pb_dts_hd = true;
        /* As we ignore the stream, do not modify those variables:
        *pi_channels = ;
        *pi_channels_conf = ;
        *pi_sample_rate = ;
        *pi_bit_rate = ;
        *pi_frame_length = ;
        */
        return i_dts_hd_size;
    }
    else
    {
        return VLC_EGENERIC;
    }
    *pb_dts_hd = false;
    return i_frame_size;
}
