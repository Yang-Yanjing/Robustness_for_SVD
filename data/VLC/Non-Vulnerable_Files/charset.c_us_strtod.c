 */
double us_strtod( const char *str, char **end )
{
    locale_t loc = newlocale (LC_NUMERIC_MASK, "C", NULL);
    locale_t oldloc = uselocale (loc);
    double res = strtod (str, end);
    if (loc != (locale_t)0)
    {
        uselocale (oldloc);
        freelocale (loc);
    }
    return res;
}
