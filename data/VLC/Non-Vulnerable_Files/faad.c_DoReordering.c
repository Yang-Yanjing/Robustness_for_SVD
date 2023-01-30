 *****************************************************************************/
static void DoReordering( uint32_t *p_out, uint32_t *p_in, int i_samples,
                          int i_nb_channels, uint32_t *pi_chan_positions )
{
    int pi_chan_table[MAX_CHANNEL_POSITIONS] = {0};
    int i, j, k;
    /* Find the channels mapping */
    for( i = 0, j = 0; i < MAX_CHANNEL_POSITIONS; i++ )
    {
        for( k = 0; k < i_nb_channels; k++ )
        {
            if( pi_channels_ordered[i] == pi_chan_positions[k] )
            {
                pi_chan_table[k] = j++;
                break;
            }
        }
    }
    /* Do the actual reordering */
    if( HAVE_FPU )
        for( i = 0; i < i_samples; i++ )
            for( j = 0; j < i_nb_channels; j++ )
                p_out[i * i_nb_channels + pi_chan_table[j]] =
                    p_in[i * i_nb_channels + j];
    else
        for( i = 0; i < i_samples; i++ )
            for( j = 0; j < i_nb_channels; j++ )
                ((uint16_t *)p_out)[i * i_nb_channels + pi_chan_table[j]] =
                    ((uint16_t *)p_in)[i * i_nb_channels + j];
}
