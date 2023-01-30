av_cold AVFloatDSPContext *avpriv_float_dsp_alloc(int bit_exact)
{
    AVFloatDSPContext *ret = av_mallocz(sizeof(AVFloatDSPContext));
    if (ret)
        avpriv_float_dsp_init(ret, bit_exact);
    return ret;
}
