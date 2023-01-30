static int
config_Write (FILE *file, const char *desc, const char *type,
              bool comment, const char *name, const char *fmt, ...)
{
    va_list ap;
    int ret;
    if (desc == NULL)
        desc = "?";
    if (fprintf (file, "# %s (%s)\n%s%s=", desc, vlc_gettext (type),
                 comment ? "#" : "", name) < 0)
        return -1;
    va_start (ap, fmt);
    ret = vfprintf (file, fmt, ap);
    va_end (ap);
    if (ret < 0)
        return -1;
    if (fputs ("\n\n", file) == EOF)
        return -1;
    return 0;
}
