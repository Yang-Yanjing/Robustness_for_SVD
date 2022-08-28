static void run_lavu_cast128(uint8_t *output,
                             const uint8_t *input, unsigned size)
{
    static struct AVCAST5 *cast;
    if (!cast && !(cast = av_cast5_alloc()))
        fatal_error("out of memory");
    av_cast5_init(cast, hardcoded_key, 128);
    av_cast5_crypt(cast, output, input, size >> 3, 0);
}
