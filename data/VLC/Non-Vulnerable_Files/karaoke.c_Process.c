}
static block_t *Process (filter_t *filter, block_t *block)
{
    const float factor = .70710678 /* 1. / sqrtf (2) */;
    float *spl = (float *)block->p_buffer;
    for (unsigned i = block->i_nb_samples; i > 0; i--)
    {
        float s = (spl[0] - spl[1]) * factor;
        *(spl++) = s;
        *(spl++) = s;
        /* TODO: set output format to mono */
    }
    (void) filter;
    return block;
}
