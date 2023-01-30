}
static int MlpParse( mlp_header_t *p_mlp, const uint8_t p_hdr[MLP_HEADER_SYNC] )
{
    bs_t s;
    assert( !memcmp( p_hdr, pu_start_code, 3 ) );
    /* TODO Checksum ? */
    /* */
    bs_init( &s, &p_hdr[3], MLP_HEADER_SYNC - 3 );
    /* Stream type */
    p_mlp->i_type = bs_read( &s, 8 );
    int i_rate_idx1;
    if( p_mlp->i_type == 0xbb )        /* MLP */
    {
        static const unsigned pu_channels[32] = {
            1, 2, 3, 4, 3, 4, 5, 3, 4, 5, 4, 5, 6, 4, 5, 4,
            5, 6, 5, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };
        bs_skip( &s, 4 + 4 );
        i_rate_idx1 = bs_read( &s, 4 );
        // Just skip the 4 following, since we don't use it
        // const int i_rate_idx2 = bs_read( &s, 4 );
        bs_skip( &s, 4 );
        bs_skip( &s, 11 );
        const int i_channel_idx = bs_read( &s, 5 );
        p_mlp->i_channels = pu_channels[i_channel_idx];
    }
    else if( p_mlp->i_type == 0xba )   /* True HD */
    {
        i_rate_idx1 = bs_read( &s, 4 );
        bs_skip( &s, 8 );
        const int i_channel1 = bs_read( &s, 5 );
        bs_skip( &s, 2 );
        const int i_channel2 = bs_read( &s, 13 );
        if( i_channel2 )
            p_mlp->i_channels = TrueHdChannels( i_channel2 );
        else
            p_mlp->i_channels = TrueHdChannels( i_channel1 );
    }
    else
    {
        return VLC_EGENERIC;
    }
    if( i_rate_idx1 == 0x0f )
        p_mlp->i_rate = 0;
    else
        p_mlp->i_rate = ( ( i_rate_idx1 & 0x8 ) ? 44100 : 48000 ) << (i_rate_idx1 & 0x7);
    p_mlp->i_channels_conf = 0; /* TODO ? */
    p_mlp->i_samples = 40 << ( i_rate_idx1 & 0x07 );
    bs_skip( &s, 48 );
    p_mlp->b_vbr = bs_read( &s, 1 );
    p_mlp->i_bitrate = ( bs_read( &s, 15 ) * p_mlp->i_rate + 8) / 16;
    p_mlp->i_substreams = bs_read( &s, 4 );
    bs_skip( &s, 4 + 11 * 8 );
    //fprintf( stderr, "i_samples = %d channels:%d rate:%d bitsrate=%d substreams=%d\n",
    //        p_mlp->i_samples, p_mlp->i_channels, p_mlp->i_rate, p_mlp->i_bitrate, p_mlp->i_substreams );
    return VLC_SUCCESS;
}
