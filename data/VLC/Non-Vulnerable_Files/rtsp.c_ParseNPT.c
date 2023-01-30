}
static int64_t ParseNPT (const char *str)
{
    locale_t loc = newlocale (LC_NUMERIC_MASK, "C", NULL);
    locale_t oldloc = uselocale (loc);
    unsigned hour, min;
    float sec;
    if (sscanf (str, "%u:%u:%f", &hour, &min, &sec) == 3)
        sec += ((hour * 60) + min) * 60;
    else
    if (sscanf (str, "%f", &sec) != 1)
        sec = -1;
    if (loc != (locale_t)0)
    {
        uselocale (oldloc);
        freelocale (loc);
    }
    return sec < 0 ? -1 : sec * CLOCK_FREQ;
}
