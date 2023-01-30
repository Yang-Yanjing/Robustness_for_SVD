static unsigned bit_depth(uint64_t mask)
{
    unsigned result = 64;
    for (; result && !(mask & 1); --result, mask >>= 1);
    return result;
}
