DEFINE_CRYPTO_WRAPPER(ripemd160, RIPEMD160)
static void run_crypto_aes128(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    AES_KEY aes;
    unsigned i;
    AES_set_encrypt_key(hardcoded_key, 128, &aes);
    size -= 15;
    for (i = 0; i < size; i += 16)
        AES_encrypt(input + i, output + i, &aes);
}
