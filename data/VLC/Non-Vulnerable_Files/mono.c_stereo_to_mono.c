/* Simple stereo to mono mixing. */
static unsigned int stereo_to_mono( filter_t *p_filter,
                                    block_t *p_output, block_t *p_input )
{
    filter_sys_t *p_sys = (filter_sys_t *)p_filter->p_sys;
    int16_t *p_in, *p_out;
    unsigned int n;
    p_in = (int16_t *) p_input->p_buffer;
    p_out = (int16_t *) p_output->p_buffer;
    for( n = 0; n < (p_input->i_nb_samples * p_sys->i_nb_channels); n++ )
    {
        /* Fake real mono. */
        if( p_sys->i_channel_selected == -1)
        {
            p_out[n] = p_out[n+1] = (p_in[n] + p_in[n+1]) >> 1;
            n++;
        }
        else if( (n % p_sys->i_nb_channels) == (unsigned int) p_sys->i_channel_selected )
        {
            p_out[n] = p_out[n+1] = p_in[n];
        }
    }
    return n;
}
