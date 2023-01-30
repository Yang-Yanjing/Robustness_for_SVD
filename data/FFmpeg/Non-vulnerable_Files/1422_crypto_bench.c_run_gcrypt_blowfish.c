static void run_gcrypt_blowfish(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    static gcry_cipher_hd_t blowfish;
    if (!blowfish)
        gcry_cipher_open(&blowfish, GCRY_CIPHER_BLOWFISH, GCRY_CIPHER_MODE_ECB, 0);
    gcry_cipher_setkey(blowfish, hardcoded_key, 16);
    gcry_cipher_encrypt(blowfish, output, size, input, size);
}
