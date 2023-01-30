}
double erand48 (unsigned short subi[3])
{
    uint64_t r = iterate48 (subi);
    return ((double)r) / 281474976710655.;
}
