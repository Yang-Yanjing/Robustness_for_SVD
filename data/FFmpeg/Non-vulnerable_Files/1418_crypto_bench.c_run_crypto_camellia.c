static void run_crypto_camellia(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    CAMELLIA_KEY camellia;
    unsigned i;
    Camellia_set_key(hardcoded_key, 128, &camellia);
    size -= 15;
    for (i = 0; i < size; i += 16)
        Camellia_ecb_encrypt(input + i, output + i, &camellia, 1);
}
