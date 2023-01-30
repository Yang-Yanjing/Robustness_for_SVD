 *****************************************************************************/
static void Interleave( float * p_out, const float * p_in, int i_nb_channels,
                        uint8_t *pi_chan_table )
{
    /* We do not only have to interleave, but also reorder the channels. */
    int i, j;
    for ( j = 0; j < i_nb_channels; j++ )
    {
        for ( i = 0; i < 256; i++ )
        {
            p_out[i * i_nb_channels + pi_chan_table[j]] = p_in[j * 256 + i];
        }
    }
}
