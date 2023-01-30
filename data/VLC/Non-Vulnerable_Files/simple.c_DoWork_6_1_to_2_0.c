}
static void DoWork_6_1_to_2_0( filter_t *p_filter, block_t *p_in_buf,
                               block_t *p_out_buf )
{
    VLC_UNUSED(p_filter);
    float *p_dest = (float *)p_out_buf->p_buffer;
    const float *p_src = (const float *)p_in_buf->p_buffer;
    for( int i = p_in_buf->i_nb_samples; i--; )
    {
        float ctr = (p_src[2] + p_src[5]) * 0.7071f;
        *p_dest++ = p_src[0] + p_src[3] + ctr;
        *p_dest++ = p_src[1] + p_src[4] + ctr;
        p_src += 6;
        /* We always have LFE here */
        p_src++;
    }
}
