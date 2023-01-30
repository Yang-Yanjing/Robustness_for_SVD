}
static int vlc_swidth(const char *str)
{
    for (int total = 0;;)
    {
        uint32_t cp;
        size_t charlen = vlc_towc(str, &cp);
        if (charlen == 0)
            return total;
        if (charlen == (size_t)-1)
            return -1;
        str += charlen;
        int w = wcwidth(cp);
        if (w == -1)
            return -1;
        total += w;
    }
}
