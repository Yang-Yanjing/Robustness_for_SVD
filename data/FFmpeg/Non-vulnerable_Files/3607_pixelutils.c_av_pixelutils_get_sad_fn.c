av_pixelutils_sad_fn av_pixelutils_get_sad_fn(int w_bits, int h_bits, int aligned, void *log_ctx)
{
#if !CONFIG_PIXELUTILS
    av_log(log_ctx, AV_LOG_ERROR, "pixelutils support is required "
           "but libavutil is not compiled with it\n");
    return NULL;
#else
    av_pixelutils_sad_fn sad[FF_ARRAY_ELEMS(sad_c)];
    memcpy(sad, sad_c, sizeof(sad));
    if (w_bits < 1 || w_bits > FF_ARRAY_ELEMS(sad) ||
        h_bits < 1 || h_bits > FF_ARRAY_ELEMS(sad))
        return NULL;
    if (w_bits != h_bits) 
        return NULL;
#if ARCH_X86
    ff_pixelutils_sad_init_x86(sad, aligned);
#endif
    return sad[w_bits - 1];
#endif
}
