static void run_gcrypt_camellia(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    static gcry_cipher_hd_t camellia;
    if (!camellia)
        gcry_cipher_open(&camellia, GCRY_CIPHER_CAMELLIA128, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(camellia, hardcoded_key, 16);
    gcry_cipher_encrypt(camellia, output, size, input, size);
}
