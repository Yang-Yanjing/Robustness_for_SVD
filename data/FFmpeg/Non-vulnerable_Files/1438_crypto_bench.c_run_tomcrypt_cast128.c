static void run_tomcrypt_cast128(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    symmetric_key cast;
    unsigned i;
    cast5_setup(hardcoded_key, 16, 0, &cast);
    for (i = 0; i < size; i += 8)
        cast5_ecb_encrypt(input + i, output + i, &cast);
}
