void av_hmac_update(AVHMAC *c, const uint8_t *data, unsigned int len)
{
    c->update(c->hash, data, len);
}
