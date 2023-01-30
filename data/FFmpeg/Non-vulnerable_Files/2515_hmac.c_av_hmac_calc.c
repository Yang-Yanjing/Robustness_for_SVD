int av_hmac_calc(AVHMAC *c, const uint8_t *data, unsigned int len,
                 const uint8_t *key, unsigned int keylen,
                 uint8_t *out, unsigned int outlen)
{
    av_hmac_init(c, key, keylen);
    av_hmac_update(c, data, len);
    return av_hmac_final(c, out, outlen);
}
