 *****************************************************************************/
static void Interleave( sample_t *restrict p_out, const sample_t *restrict p_in,
                        unsigned i_nb_channels, uint8_t *restrict pi_chan_table )
{
    /* We do not only have to interleave, but also reorder the channels */
    for( unsigned j = 0; j < i_nb_channels; j++ )
    {
        for( unsigned i = 0; i < 256; i++ )
        {
#ifdef LIBA52_FIXED
            union { uint32_t u; int32_t i; } spl;
            spl.u = ((uint32_t)p_in[j * 256 + i]) << 4;
            p_out[i * i_nb_channels + pi_chan_table[j]] = spl.i;
#else
            p_out[i * i_nb_channels + pi_chan_table[j]] = p_in[j * 256 + i];
#endif
        }
    }
}
