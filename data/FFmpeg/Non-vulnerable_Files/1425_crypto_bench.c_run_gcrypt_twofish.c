static void run_gcrypt_twofish(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    static gcry_cipher_hd_t twofish;
    if (!twofish)
        gcry_cipher_open(&twofish, GCRY_CIPHER_TWOFISH128, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(twofish, hardcoded_key, 16);
    gcry_cipher_encrypt(twofish, output, size, input, size);
}
