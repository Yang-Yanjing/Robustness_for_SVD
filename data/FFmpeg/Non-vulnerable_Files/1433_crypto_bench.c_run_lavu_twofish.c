static void run_lavu_twofish(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    static struct AVTWOFISH *twofish;
    if (!twofish && !(twofish = av_twofish_alloc()))
        fatal_error("out of memory");
    av_twofish_init(twofish, hardcoded_key, 128);
    av_twofish_crypt(twofish, output, input, size >> 4, NULL, 0);
}
