void av_hmac_free(AVHMAC *c)
{
    if (!c)
        return;
    av_freep(&c->hash);
    av_free(c);
}
