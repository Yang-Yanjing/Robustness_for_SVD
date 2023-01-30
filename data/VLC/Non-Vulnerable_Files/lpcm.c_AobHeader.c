};
static int AobHeader( unsigned *pi_rate,
                      unsigned *pi_channels, unsigned *pi_layout,
                      unsigned *pi_bits,
                      unsigned *pi_padding,
                      aob_group_t g[2],
                      const uint8_t *p_header )
{
    const unsigned i_header_size = GetWBE( &p_header[1] );
    if( i_header_size + 3 < LPCM_AOB_HEADER_LEN )
        return VLC_EGENERIC;
    *pi_padding = 3+i_header_size - LPCM_AOB_HEADER_LEN;
    const int i_index_size_g1 = (p_header[6] >> 4) & 0x0f;
    const int i_index_size_g2 = (p_header[6]     ) & 0x0f;
    const int i_index_rate_g1 = (p_header[7] >> 4) & 0x0f;
    const int i_index_rate_g2 = (p_header[7]     ) & 0x0f;
    const int i_assignment     = p_header[9];
    /* Validate */
    if( i_index_size_g1 > 0x02 ||
        ( i_index_size_g2 != 0x0f && i_index_size_g2 > 0x02 ) )
        return VLC_EGENERIC;
    if( (i_index_rate_g1 & 0x07) > 0x02 ||
        ( i_index_rate_g2 != 0x0f && (i_index_rate_g1 & 0x07) > 0x02 ) )
        return VLC_EGENERIC;
    if( i_assignment > 20 )
        return VLC_EGENERIC;
    /* */
    *pi_bits = 16 + 4 * i_index_size_g1;
    if( i_index_rate_g1 & 0x08 )
        *pi_rate = 44100 << (i_index_rate_g1 & 0x07);
    else
        *pi_rate = 48000 << (i_index_rate_g1 & 0x07);
    /* Group1 */
    unsigned i_channels1 = 0;
    unsigned i_layout1 = 0;
    for( int i = 0; p_aob_group1[i_assignment][i] != 0; i++ )
    {
        i_channels1++;
        i_layout1 |= p_aob_group1[i_assignment][i];
    }
    /* Group2 */
    unsigned i_channels2 = 0;
    unsigned i_layout2 = 0;
    if( i_index_size_g2 != 0x0f && i_index_rate_g2 != 0x0f )
    {
        for( int i = 0; p_aob_group2[i_assignment][i] != 0; i++ )
        {
            i_channels2++;
            i_layout2 |= p_aob_group2[i_assignment][i];
        }
        assert( (i_layout1 & i_layout2) == 0 );
    }
    /* It is enabled only when presents and compatible wih group1 */
    const bool b_group2_used = i_index_size_g1 == i_index_size_g2 &&
                               i_index_rate_g1 == i_index_rate_g2;
    /* */
    *pi_channels = i_channels1 + ( b_group2_used ? i_channels2 : 0 );
    *pi_layout   = i_layout1   | ( b_group2_used ? i_layout2   : 0 );
    /* */
    for( unsigned i = 0; i < 2; i++ )
    {
        const unsigned *p_aob = i == 0 ? p_aob_group1[i_assignment] :
                                         p_aob_group2[i_assignment];
        g[i].i_channels = i == 0 ? i_channels1 :
                                   i_channels2;
        g[i].b_used = i == 0 || b_group2_used;
        if( !g[i].b_used )
            continue;
        for( unsigned j = 0; j < g[i].i_channels; j++ )
        {
            g[i].pi_position[j] = 0;
            for( int k = 0; pi_vlc_chan_order_wg4[k] != 0; k++ )
            {
                const unsigned i_channel = pi_vlc_chan_order_wg4[k];
                if( i_channel == p_aob[j] )
                    break;
                if( (*pi_layout) & i_channel )
                    g[i].pi_position[j]++;
            }
        }
    }
    return VLC_SUCCESS;
}
