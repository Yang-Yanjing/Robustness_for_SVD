void av_md5_final(AVMD5 *ctx, uint8_t *dst)
{
    int i;
    uint64_t finalcount = av_le2ne64(ctx->len << 3);
    av_md5_update(ctx, "\200", 1);
    while ((ctx->len & 63) != 56)
        av_md5_update(ctx, "", 1);
    av_md5_update(ctx, (uint8_t *)&finalcount, 8);
    for (i = 0; i < 4; i++)
        AV_WL32(dst + 4*i, ctx->ABCD[3 - i]);
}
