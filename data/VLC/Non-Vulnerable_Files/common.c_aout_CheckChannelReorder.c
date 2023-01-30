 *****************************************************************************/
unsigned aout_CheckChannelReorder( const uint32_t *chans_in,
                                   const uint32_t *chans_out,
                                   uint32_t mask, uint8_t *restrict table )
{
    unsigned channels = 0;
    if( chans_in == NULL )
        chans_in = pi_vlc_chan_order_wg4;
    if( chans_out == NULL )
        chans_out = pi_vlc_chan_order_wg4;
    for( unsigned i = 0; chans_in[i]; i++ )
    {
        const uint32_t chan = chans_in[i];
        if( !(mask & chan) )
            continue;
        unsigned index = 0;
        for( unsigned j = 0; chan != chans_out[j]; j++ )
            if( mask & chans_out[j] )
                index++;
        table[channels++] = index;
    }
    for( unsigned i = 0; i < channels; i++ )
        if( table[i] != i )
            return channels;
    return 0;
}
