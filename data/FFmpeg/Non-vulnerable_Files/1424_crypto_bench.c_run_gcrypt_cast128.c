static void run_gcrypt_cast128(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    static gcry_cipher_hd_t cast;
    if (!cast)
        gcry_cipher_open(&cast, GCRY_CIPHER_CAST5, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(cast, hardcoded_key, 16);
    gcry_cipher_encrypt(cast, output, size, input, size);
}
