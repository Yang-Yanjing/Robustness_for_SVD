static void SET_TYPE(resample_one)(ResampleContext *c,
                                   void *dst0, int dst_index, const void *src0,
                                   unsigned int index, int frac)
{
    FELEM *dst = dst0;
    const FELEM *src = src0;
    int i;
    unsigned int sample_index = index >> c->phase_shift;
    FELEM2 val = 0;
    FELEM *filter = ((FELEM *)c->filter_bank) +
                    c->filter_length * (index & c->phase_mask);
    for (i = 0; i < c->filter_length; i++)
        val += src[sample_index + i] * (FELEM2)filter[i];
    OUT(dst[dst_index], val);
}
