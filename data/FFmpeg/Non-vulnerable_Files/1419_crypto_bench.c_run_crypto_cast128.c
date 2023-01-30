static void run_crypto_cast128(uint8_t *output,
                               const uint8_t *input, unsigned size)
{
    CAST_KEY cast;
    unsigned i;
    CAST_set_key(&cast, 16, hardcoded_key);
    for (i = 0; i < size; i += 8)
        CAST_ecb_encrypt(input + i, output + i, &cast, 1);
}
