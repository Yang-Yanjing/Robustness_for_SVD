}
char *sdp_AddAttribute (char **sdp, const char *name, const char *fmt, ...)
{
    char *ret;
    va_list ap;
    va_start (ap, fmt);
    ret = vsdp_AddAttribute (sdp, name, fmt, ap);
    va_end (ap);
    return ret;
}
