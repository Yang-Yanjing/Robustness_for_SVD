int av_hash_alloc(AVHashContext **ctx, const char *name)
{
    AVHashContext *res;
    int i;
    *ctx = NULL;
    for (i = 0; i < NUM_HASHES; i++)
        if (av_strcasecmp(name, hashdesc[i].name) == 0)
            break;
    if (i >= NUM_HASHES) return AVERROR(EINVAL);
    res = av_mallocz(sizeof(*res));
    if (!res) return AVERROR(ENOMEM);
    res->type = i;
    switch (i) {
    case MD5:     res->ctx = av_md5_alloc(); break;
    case MURMUR3: res->ctx = av_murmur3_alloc(); break;
    case RIPEMD128:
    case RIPEMD160:
    case RIPEMD256:
    case RIPEMD320: res->ctx = av_ripemd_alloc(); break;
    case SHA160:
    case SHA224:
    case SHA256:  res->ctx = av_sha_alloc(); break;
    case SHA512_224:
    case SHA512_256:
    case SHA384:
    case SHA512:  res->ctx = av_sha512_alloc(); break;
    case CRC32:   res->crctab = av_crc_get_table(AV_CRC_32_IEEE_LE); break;
    case ADLER32: break;
    }
    if (i != ADLER32 && i != CRC32 && !res->ctx) {
        av_free(res);
        return AVERROR(ENOMEM);
    }
    *ctx = res;
    return 0;
}
