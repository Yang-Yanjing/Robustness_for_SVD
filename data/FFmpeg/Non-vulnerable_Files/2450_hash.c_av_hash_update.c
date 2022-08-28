void av_hash_update(AVHashContext *ctx, const uint8_t *src, int len)
{
    switch (ctx->type) {
    case MD5:     av_md5_update(ctx->ctx, src, len); break;
    case MURMUR3: av_murmur3_update(ctx->ctx, src, len); break;
    case RIPEMD128:
    case RIPEMD160:
    case RIPEMD256:
    case RIPEMD320: av_ripemd_update(ctx->ctx, src, len); break;
    case SHA160:
    case SHA224:
    case SHA256:  av_sha_update(ctx->ctx, src, len); break;
    case SHA512_224:
    case SHA512_256:
    case SHA384:
    case SHA512:  av_sha512_update(ctx->ctx, src, len); break;
    case CRC32:   ctx->crc = av_crc(ctx->crctab, ctx->crc, src, len); break;
    case ADLER32: ctx->crc = av_adler32_update(ctx->crc, src, len); break;
    }
}
