static inline double logdb(uint64_t v)
{
    double d = v / (double)(0x8000 * 0x8000);
    if (!v)
        return MAX_DB;
    return log(d) * -4.3429448190325182765112891891660508229; 
}
