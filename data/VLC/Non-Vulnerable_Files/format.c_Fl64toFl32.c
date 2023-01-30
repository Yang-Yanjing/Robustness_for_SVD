}
static block_t *Fl64toFl32(filter_t *filter, block_t *b)
{
    double *src = (double *)b->p_buffer;
    float  *dst = (float *)src;
    for (size_t i = b->i_buffer / 8; i--;)
        *(dst++) = *(src++);
    VLC_UNUSED(filter);
    return b;
}
