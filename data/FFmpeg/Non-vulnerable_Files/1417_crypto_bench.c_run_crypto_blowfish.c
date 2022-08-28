static void run_crypto_blowfish(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    BF_KEY blowfish;
    unsigned i;
    BF_set_key(&blowfish, 16, hardcoded_key);
    for (i = 0; i < size; i += 8)
        BF_ecb_encrypt(input + i, output + i, &blowfish, 1);
}
