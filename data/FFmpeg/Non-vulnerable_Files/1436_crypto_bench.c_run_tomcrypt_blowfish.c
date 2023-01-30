static void run_tomcrypt_blowfish(uint8_t *output,
                                  const uint8_t *input, unsigned size)
{
    symmetric_key blowfish;
    unsigned i;
    blowfish_setup(hardcoded_key, 16, 0, &blowfish);
    for (i = 0; i < size; i += 8)
        blowfish_ecb_encrypt(input + i, output + i, &blowfish);
}
