void av_hash_final(AVHashContext *ctx, uint8_t *dst)
{
    switch (ctx->type) {
    case MD5:     av_md5_final(ctx->ctx, dst); break;
    case MURMUR3: av_murmur3_final(ctx->ctx, dst); break;
    case RIPEMD128:
    case RIPEMD160:
    case RIPEMD256:
    case RIPEMD320: av_ripemd_final(ctx->ctx, dst); break;
    case SHA160:
    case SHA224:
    case SHA256:  av_sha_final(ctx->ctx, dst); break;
    case SHA512_224:
    case SHA512_256:
    case SHA384:
    case SHA512:  av_sha512_final(ctx->ctx, dst); break;
    case CRC32:   AV_WB32(dst, ctx->crc ^ UINT32_MAX); break;
    case ADLER32: AV_WB32(dst, ctx->crc); break;
    }
}
