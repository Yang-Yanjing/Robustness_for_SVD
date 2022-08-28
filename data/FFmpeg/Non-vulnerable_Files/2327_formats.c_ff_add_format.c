int ff_add_format(AVFilterFormats **avff, int64_t fmt)
{
    ADD_FORMAT(avff, fmt, int, formats, nb_formats);
    return 0;
}
