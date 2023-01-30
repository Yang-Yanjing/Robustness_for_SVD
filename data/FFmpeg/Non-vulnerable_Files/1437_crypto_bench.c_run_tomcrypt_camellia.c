static void run_tomcrypt_camellia(uint8_t *output,
                                  const uint8_t *input, unsigned size)
{
    symmetric_key camellia;
    unsigned i;
    camellia_setup(hardcoded_key, 16, 0, &camellia);
    size -= 15;
    for (i = 0; i < size; i += 16)
        camellia_ecb_encrypt(input + i, output + i, &camellia);
}
