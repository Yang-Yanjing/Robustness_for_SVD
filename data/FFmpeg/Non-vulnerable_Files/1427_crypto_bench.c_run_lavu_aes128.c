DEFINE_LAVU_MD(ripemd160, AVRIPEMD, ripemd, 160);
static void run_lavu_aes128(uint8_t *output,
                            const uint8_t *input, unsigned size)
{
    static struct AVAES *aes;
    if (!aes && !(aes = av_aes_alloc()))
        fatal_error("out of memory");
    av_aes_init(aes, hardcoded_key, 128, 0);
    av_aes_crypt(aes, output, input, size >> 4, NULL, 0);
}
