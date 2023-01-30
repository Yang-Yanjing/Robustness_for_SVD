static void test(AVHMAC *hmac, const uint8_t *key, int keylen,
                 const uint8_t *data, int datalen)
{
    uint8_t buf[MAX_HASHLEN];
    int out, i;
    
    
    if (!key[keylen - 1])
        keylen--;
    if (!data[datalen - 1])
        datalen--;
    out = av_hmac_calc(hmac, data, datalen, key, keylen, buf, sizeof(buf));
    for (i = 0; i < out; i++)
        printf("%02x", buf[i]);
    printf("\n");
}
