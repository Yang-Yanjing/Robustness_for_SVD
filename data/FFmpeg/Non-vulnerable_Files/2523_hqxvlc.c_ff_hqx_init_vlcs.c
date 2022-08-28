av_cold int ff_hqx_init_vlcs(HQXContext *ctx)
{
    int ret = init_vlc(&ctx->cbp_vlc, 5, FF_ARRAY_ELEMS(cbp_vlc_lens),
                       cbp_vlc_lens, 1, 1, cbp_vlc_bits, 1, 1, 0);
    if (ret < 0)
        return ret;
    INIT_DC_TABLE(0, dc9);
    INIT_DC_TABLE(1, dc10);
    INIT_DC_TABLE(2, dc11);
    return 0;
}
