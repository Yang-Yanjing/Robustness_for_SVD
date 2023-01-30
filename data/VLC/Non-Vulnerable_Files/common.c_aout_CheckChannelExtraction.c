}
bool aout_CheckChannelExtraction( int *pi_selection,
                                  uint32_t *pi_layout, int *pi_channels,
                                  const uint32_t pi_order_dst[AOUT_CHAN_MAX],
                                  const uint32_t *pi_order_src, int i_channels )
{
    const uint32_t pi_order_dual_mono[] = { AOUT_CHAN_LEFT, AOUT_CHAN_RIGHT };
    uint32_t i_layout = 0;
    int i_out = 0;
    int pi_index[AOUT_CHAN_MAX];
    /* */
    if( !pi_order_dst )
        pi_order_dst = pi_vlc_chan_order_wg4;
    /* Detect special dual mono case */
    if( i_channels == 2 &&
        pi_order_src[0] == AOUT_CHAN_CENTER && pi_order_src[1] == AOUT_CHAN_CENTER )
    {
        i_layout |= AOUT_CHAN_DUALMONO;
        pi_order_src = pi_order_dual_mono;
    }
    /* */
    for( int i = 0; i < i_channels; i++ )
    {
        /* Ignore unknown or duplicated channels or not present in output */
        if( !pi_order_src[i] || (i_layout & pi_order_src[i]) )
            continue;
        for( int j = 0; j < AOUT_CHAN_MAX; j++ )
        {
            if( pi_order_dst[j] == pi_order_src[i] )
            {
                assert( i_out < AOUT_CHAN_MAX );
                pi_index[i_out++] = i;
                i_layout |= pi_order_src[i];
                break;
            }
        }
    }
    /* */
    for( int i = 0, j = 0; i < AOUT_CHAN_MAX; i++ )
    {
        for( int k = 0; k < i_out; k++ )
        {
            if( pi_order_dst[i] == pi_order_src[pi_index[k]] )
            {
                pi_selection[j++] = pi_index[k];
                break;
            }
        }
    }
    *pi_layout = i_layout;
    *pi_channels = i_out;
    for( int i = 0; i < i_out; i++ )
    {
        if( pi_selection[i] != i )
            return true;
    }
    return i_out != i_channels;
}
