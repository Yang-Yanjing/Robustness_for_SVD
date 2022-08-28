static void run_lavu_xtea(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    static struct AVXTEA *xtea;
    if (!xtea && !(xtea = av_xtea_alloc()))
        fatal_error("out of memory");
    av_xtea_init(xtea, hardcoded_key);
    av_xtea_crypt(xtea, output, input, size >> 3, NULL, 0);
}
