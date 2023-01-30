}
long nrand48 (unsigned short subi[3])
{
    return iterate48 (subi) >> 17;
}
