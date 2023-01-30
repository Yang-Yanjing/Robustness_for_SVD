static inline void addkey_d(uint8_t *dst, const av_aes_block *src,
                            const av_aes_block *round_key)
{
    AV_WN64(dst,     src->u64[0] ^ round_key->u64[0]);
    AV_WN64(dst + 8, src->u64[1] ^ round_key->u64[1]);
}
