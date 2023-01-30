static double midi(void *p, double f)
{
    return log2(f/440.0) * 12.0 + 69.0;
}
