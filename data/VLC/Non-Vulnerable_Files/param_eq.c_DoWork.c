 *****************************************************************************/
static block_t *DoWork( filter_t * p_filter, block_t * p_in_buf )
{
    ProcessEQ( (float*)p_in_buf->p_buffer, (float*)p_in_buf->p_buffer,
               p_filter->p_sys->p_state,
               p_filter->fmt_in.audio.i_channels, p_in_buf->i_nb_samples,
               p_filter->p_sys->coeffs, 5 );
    return p_in_buf;
}
