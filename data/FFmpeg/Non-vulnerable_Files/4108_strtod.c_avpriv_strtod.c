double strtod(const char *, char **);
double avpriv_strtod(const char *nptr, char **endptr)
{
    char *end;
    double res;
    
    while (av_isspace(*nptr))
        nptr++;
    if (!av_strncasecmp(nptr, "infinity", 8)) {
        end = nptr + 8;
        res = INFINITY;
    } else if (!av_strncasecmp(nptr, "inf", 3)) {
        end = nptr + 3;
        res = INFINITY;
    } else if (!av_strncasecmp(nptr, "+infinity", 9)) {
        end = nptr + 9;
        res = INFINITY;
    } else if (!av_strncasecmp(nptr, "+inf", 4)) {
        end = nptr + 4;
        res = INFINITY;
    } else if (!av_strncasecmp(nptr, "-infinity", 9)) {
        end = nptr + 9;
        res = -INFINITY;
    } else if (!av_strncasecmp(nptr, "-inf", 4)) {
        end = nptr + 4;
        res = -INFINITY;
    } else if (!av_strncasecmp(nptr, "nan", 3)) {
        end = check_nan_suffix(nptr + 3);
        res = NAN;
    } else if (!av_strncasecmp(nptr, "+nan", 4) ||
               !av_strncasecmp(nptr, "-nan", 4)) {
        end = check_nan_suffix(nptr + 4);
        res = NAN;
    } else if (!av_strncasecmp(nptr, "0x", 2) ||
               !av_strncasecmp(nptr, "-0x", 3) ||
               !av_strncasecmp(nptr, "+0x", 3)) {
        

        res = strtoll(nptr, &end, 16);
    } else {
        res = strtod(nptr, &end);
    }
    if (endptr)
        *endptr = end;
    return res;
}
