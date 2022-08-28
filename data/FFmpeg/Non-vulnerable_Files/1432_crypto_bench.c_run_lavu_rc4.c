static void run_lavu_rc4(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    static struct AVRC4 *rc4;
    if (!rc4 && !(rc4 = av_rc4_alloc()))
        fatal_error("out of memory");
    av_rc4_init(rc4, hardcoded_key, 128, 0);
    av_rc4_crypt(rc4, output, input, size, NULL, 0);
}
