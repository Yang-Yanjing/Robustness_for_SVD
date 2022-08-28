static const char *cpu_suffix(int cpu)
{
    int i = FF_ARRAY_ELEMS(cpus);
    while (--i >= 0)
        if (cpu & cpus[i].flag)
            return cpus[i].suffix;
    return "c";
}
