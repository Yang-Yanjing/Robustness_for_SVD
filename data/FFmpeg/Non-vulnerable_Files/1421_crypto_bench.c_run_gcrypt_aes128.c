DEFINE_GCRYPT_WRAPPER(ripemd160, RMD160)
static void run_gcrypt_aes128(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    static gcry_cipher_hd_t aes;
    if (!aes)
        gcry_cipher_open(&aes, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(aes, hardcoded_key, 16);
    gcry_cipher_encrypt(aes, output, size, input, size);
}
