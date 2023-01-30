                          int16_t *temporal);
av_cold void ff_hqdn3d_init_x86(HQDN3DContext *hqdn3d)
{
#if HAVE_YASM
    hqdn3d->denoise_row[8]  = ff_hqdn3d_row_8_x86;
    hqdn3d->denoise_row[9]  = ff_hqdn3d_row_9_x86;
    hqdn3d->denoise_row[10] = ff_hqdn3d_row_10_x86;
    hqdn3d->denoise_row[16] = ff_hqdn3d_row_16_x86;
#endif 
}
