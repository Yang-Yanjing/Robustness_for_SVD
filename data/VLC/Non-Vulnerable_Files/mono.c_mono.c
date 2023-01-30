/* Simple stereo to mono mixing. */
static unsigned int mono( filter_t *p_filter,
                          block_t *p_output, block_t *p_input )
{
    filter_sys_t *p_sys = (filter_sys_t *)p_filter->p_sys;
    int16_t *p_in, *p_out;
    unsigned int n = 0, r = 0;
    p_in = (int16_t *) p_input->p_buffer;
    p_out = (int16_t *) p_output->p_buffer;
    while( n < (p_input->i_nb_samples * p_sys->i_nb_channels) )
    {
        p_out[r] = (p_in[n] + p_in[n+1]) >> 1;
        r++;
        n += 2;
    }
    return r;
}
