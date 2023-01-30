void av_ripemd_final(AVRIPEMD* ctx, uint8_t *digest)
{
    int i;
    uint64_t finalcount = av_le2ne64(ctx->count << 3);
    av_ripemd_update(ctx, "\200", 1);
    while ((ctx->count & 63) != 56)
        av_ripemd_update(ctx, "", 1);
    av_ripemd_update(ctx, (uint8_t *)&finalcount, 8); 
    for (i = 0; i < ctx->digest_len; i++)
        AV_WL32(digest + i*4, ctx->state[i]);
}
