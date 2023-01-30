static inline void addkey_s(av_aes_block *dst, const uint8_t *src,
                            const av_aes_block *round_key)
{
    dst->u64[0] = AV_RN64(src)     ^ round_key->u64[0];
    dst->u64[1] = AV_RN64(src + 8) ^ round_key->u64[1];
}
