int av_hmac_final(AVHMAC *c, uint8_t *out, unsigned int outlen)
{
    uint8_t block[MAX_BLOCKLEN];
    int i;
    if (outlen < c->hashlen)
        return AVERROR(EINVAL);
    c->final(c->hash, out);
    c->init(c->hash);
    for (i = 0; i < c->keylen; i++)
        block[i] = c->key[i] ^ 0x5C;
    for (i = c->keylen; i < c->blocklen; i++)
        block[i] = 0x5C;
    c->update(c->hash, block, c->blocklen);
    c->update(c->hash, out, c->hashlen);
    c->final(c->hash, out);
    return c->hashlen;
}
