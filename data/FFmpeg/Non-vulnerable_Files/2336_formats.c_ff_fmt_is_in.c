int ff_fmt_is_in(int fmt, const int *fmts)
{
    const int *p;
    for (p = fmts; *p != -1; p++) {
        if (fmt == *p)
            return 1;
    }
    return 0;
}
