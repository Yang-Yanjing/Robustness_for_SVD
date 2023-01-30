}
long jrand48 (unsigned short subi[3])
{
    return ((int64_t)iterate48 (subi)) >> 16;
}
