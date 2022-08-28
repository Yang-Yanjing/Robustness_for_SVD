static void run_lavu_blowfish(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    static struct AVBlowfish *blowfish;
    if (!blowfish && !(blowfish = av_blowfish_alloc()))
        fatal_error("out of memory");
    av_blowfish_init(blowfish, hardcoded_key, 16);
    av_blowfish_crypt(blowfish, output, input, size >> 3, NULL, 0);
}
