void av_hash_init(AVHashContext *ctx)
{
    switch (ctx->type) {
    case MD5:     av_md5_init(ctx->ctx); break;
    case MURMUR3: av_murmur3_init(ctx->ctx); break;
    case RIPEMD128: av_ripemd_init(ctx->ctx, 128); break;
    case RIPEMD160: av_ripemd_init(ctx->ctx, 160); break;
    case RIPEMD256: av_ripemd_init(ctx->ctx, 256); break;
    case RIPEMD320: av_ripemd_init(ctx->ctx, 320); break;
    case SHA160:  av_sha_init(ctx->ctx, 160); break;
    case SHA224:  av_sha_init(ctx->ctx, 224); break;
    case SHA256:  av_sha_init(ctx->ctx, 256); break;
    case SHA512_224:  av_sha512_init(ctx->ctx, 224); break;
    case SHA512_256:  av_sha512_init(ctx->ctx, 256); break;
    case SHA384:  av_sha512_init(ctx->ctx, 384); break;
    case SHA512:  av_sha512_init(ctx->ctx, 512); break;
    case CRC32:   ctx->crc = UINT32_MAX; break;
    case ADLER32: ctx->crc = 1; break;
    }
}
