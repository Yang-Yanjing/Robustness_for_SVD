char *av_d2str(double d)
{
    char *str = av_malloc(16);
    if (str)
        snprintf(str, 16, "%f", d);
    return str;
}
