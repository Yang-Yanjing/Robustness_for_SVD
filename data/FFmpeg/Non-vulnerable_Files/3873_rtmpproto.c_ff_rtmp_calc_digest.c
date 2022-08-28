int ff_rtmp_calc_digest(const uint8_t *src, int len, int gap,
                        const uint8_t *key, int keylen, uint8_t *dst)
{
    AVHMAC *hmac;
    hmac = av_hmac_alloc(AV_HMAC_SHA256);
    if (!hmac)
        return AVERROR(ENOMEM);
    av_hmac_init(hmac, key, keylen);
    if (gap <= 0) {
        av_hmac_update(hmac, src, len);
    } else { 
        av_hmac_update(hmac, src, gap);
        av_hmac_update(hmac, src + gap + 32, len - gap - 32);
    }
    av_hmac_final(hmac, dst, 32);
    av_hmac_free(hmac);
    return 0;
}
