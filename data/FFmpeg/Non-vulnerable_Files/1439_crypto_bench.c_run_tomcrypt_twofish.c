static void run_tomcrypt_twofish(uint8_t *output,
                                const uint8_t *input, unsigned size)
{
    symmetric_key twofish;
    unsigned i;
    twofish_setup(hardcoded_key, 16, 0, &twofish);
    size -= 15;
    for (i = 0; i < size; i += 16)
        twofish_ecb_encrypt(input + i, output + i, &twofish);
}
