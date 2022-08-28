static void run_tomcrypt_xtea(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    symmetric_key xtea;
    unsigned i;
    xtea_setup(hardcoded_key, 16, 0, &xtea);
    for (i = 0; i < size; i += 8)
        xtea_ecb_encrypt(input + i, output + i, &xtea);
}
