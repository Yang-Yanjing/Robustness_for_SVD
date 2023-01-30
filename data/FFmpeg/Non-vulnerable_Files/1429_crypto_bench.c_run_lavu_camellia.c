static void run_lavu_camellia(uint8_t *output,
                              const uint8_t *input, unsigned size)
{
    static struct AVCAMELLIA *camellia;
    if (!camellia && !(camellia = av_camellia_alloc()))
        fatal_error("out of memory");
    av_camellia_init(camellia, hardcoded_key, 128);
    av_camellia_crypt(camellia, output, input, size >> 4, NULL, 0);
}
