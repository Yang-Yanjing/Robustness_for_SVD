static int ebml_num_size(uint64_t num)
{
    int bytes = 1;
    while ((num + 1) >> bytes * 7)
        bytes++;
    return bytes;
}
