DEFINE_TOMCRYPT_WRAPPER(ripemd160, rmd160, RIPEMD160)
static void run_tomcrypt_aes128(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    symmetric_key aes;
    unsigned i;
    aes_setup(hardcoded_key, 16, 0, &aes);
    size -= 15;
    for (i = 0; i < size; i += 16)
        aes_ecb_encrypt(input + i, output + i, &aes);
}
