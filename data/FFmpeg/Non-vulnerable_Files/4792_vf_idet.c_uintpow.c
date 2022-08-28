static uint64_t uintpow(uint64_t b,unsigned int e)
{
    uint64_t r=1;
    while(e--) r*=b;
    return r;
}
