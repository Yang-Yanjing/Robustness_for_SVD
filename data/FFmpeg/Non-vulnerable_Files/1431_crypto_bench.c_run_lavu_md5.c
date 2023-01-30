static void run_lavu_md5(uint8_t *output,
                         const uint8_t *input, unsigned size)
{
    av_md5_sum(output, input, size);
}
