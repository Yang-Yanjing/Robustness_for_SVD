static inline void addkey(av_aes_block *dst, const av_aes_block *src,
                          const av_aes_block *round_key)
{
    dst->u64[0] = src->u64[0] ^ round_key->u64[0];
    dst->u64[1] = src->u64[1] ^ round_key->u64[1];
}
