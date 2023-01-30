static void run_crypto_rc4(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    RC4_KEY rc4;
    RC4_set_key(&rc4, 16, hardcoded_key);
    RC4(&rc4, size, input, output);
}
