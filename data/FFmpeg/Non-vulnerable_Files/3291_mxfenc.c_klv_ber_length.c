static int klv_ber_length(uint64_t len)
{
    if (len < 128)
        return 1;
    else
        return (av_log2(len) >> 3) + 2;
}
