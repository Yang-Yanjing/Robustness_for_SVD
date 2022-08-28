int main(void)
{
    uint8_t key1[20], key3[131], data3[50];
    AVHMAC *hmac;
    enum AVHMACType i;
    static const uint8_t key2[]  = "Jefe";
    static const uint8_t data1[] = "Hi There";
    static const uint8_t data2[] = "what do ya want for nothing?";
    static const uint8_t data4[] = "Test Using Larger Than Block-Size Key - Hash Key First";
    static const uint8_t data5[] = "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data";
    static const uint8_t data6[] = "This is a test using a larger than block-size key and a larger "
                            "than block-size data. The key needs to be hashed before being used"
                            " by the HMAC algorithm.";
    memset(key1, 0x0b, sizeof(key1));
    memset(key3, 0xaa, sizeof(key3));
    memset(data3, 0xdd, sizeof(data3));
    
    for (i = AV_HMAC_MD5; i <= AV_HMAC_SHA1; i++) {
        hmac = av_hmac_alloc(i);
        if (!hmac)
            return 1;
        
        test(hmac, key1, hmac->hashlen, data1, sizeof(data1));
        test(hmac, key2, sizeof(key2),  data2, sizeof(data2));
        test(hmac, key3, hmac->hashlen, data3, sizeof(data3));
        test(hmac, key3, 80,            data4, sizeof(data4));
        test(hmac, key3, 80,            data5, sizeof(data5));
        av_hmac_free(hmac);
    }
    
    for (i = AV_HMAC_SHA224; i <= AV_HMAC_SHA256; i++) {
        hmac = av_hmac_alloc(i);
        if (!hmac)
            return 1;
        
        test(hmac, key1, sizeof(key1), data1, sizeof(data1));
        test(hmac, key2, sizeof(key2), data2, sizeof(data2));
        test(hmac, key3, 20,           data3, sizeof(data3));
        test(hmac, key3, sizeof(key3), data4, sizeof(data4));
        test(hmac, key3, sizeof(key3), data6, sizeof(data6));
        av_hmac_free(hmac);
    }
    for (i = AV_HMAC_SHA384; i <= AV_HMAC_SHA512; i++) {
        hmac = av_hmac_alloc(i);
        if (!hmac)
            return 1;
        
        test(hmac, key1, sizeof(key1), data1, sizeof(data1));
        test(hmac, key2, sizeof(key2), data2, sizeof(data2));
        test(hmac, key3, 20, data3, sizeof(data3));
        test(hmac, key3, sizeof(key3), data4, sizeof(data4));
        test(hmac, key3, sizeof(key3), data6, sizeof(data6));
        av_hmac_free(hmac);
    }
    return 0;
}
