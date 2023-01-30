}
static void mvnprintw(int y, int x, int w, const char *p_fmt, ...)
{
    va_list  vl_args;
    char    *p_buf;
    int      len;
    if (w <= 0)
        return;
    va_start(vl_args, p_fmt);
    int i_ret = vasprintf(&p_buf, p_fmt, vl_args);
    va_end(vl_args);
    if (i_ret == -1)
        return;
    len = strlen(p_buf);
    wchar_t wide[len + 1];
    EnsureUTF8(p_buf);
    size_t i_char_len = mbstowcs(wide, p_buf, len);
    size_t i_width; /* number of columns */
    if (i_char_len == (size_t)-1) /* an invalid character was encountered */ {
        free(p_buf);
        return;
    }
    i_width = wcswidth(wide, i_char_len);
    if (i_width == (size_t)-1) {
        /* a non printable character was encountered */
        i_width = 0;
        for (unsigned i = 0 ; i < i_char_len ; i++) {
            int i_cwidth = wcwidth(wide[i]);
            if (i_cwidth != -1)
                i_width += i_cwidth;
        }
    }
    if (i_width <= (size_t)w) {
        mvprintw(y, x, "%s", p_buf);
        mvhline(y, x + i_width, ' ', w - i_width);
        free(p_buf);
        return;
    }
    int i_total_width = 0;
    int i = 0;
    while (i_total_width < w) {
        i_total_width += wcwidth(wide[i]);
        if (w > 7 && i_total_width >= w/2) {
            wide[i  ] = '.';
            wide[i+1] = '.';
            i_total_width -= wcwidth(wide[i]) - 2;
            if (i > 0) {
                /* we require this check only if at least one character
                 * 4 or more columns wide exists (which i doubt) */
                wide[i-1] = '.';
                i_total_width -= wcwidth(wide[i-1]) - 1;
            }
            /* find the widest string */
            int j, i_2nd_width = 0;
            for (j = i_char_len - 1; i_2nd_width < w - i_total_width; j--)
                i_2nd_width += wcwidth(wide[j]);
            /* we already have i_total_width columns filled, and we can't
             * have more than w columns */
            if (i_2nd_width > w - i_total_width)
                j++;
            wmemmove(&wide[i+2], &wide[j+1], i_char_len - j - 1);
            wide[i + 2 + i_char_len - j - 1] = '\0';
            break;
        }
        i++;
    }
    if (w <= 7) /* we don't add the '...' else we lose too much chars */
        wide[i] = '\0';
    size_t i_wlen = wcslen(wide) * 6 + 1; /* worst case */
    char ellipsized[i_wlen];
    wcstombs(ellipsized, wide, i_wlen);
    mvprintw(y, x, "%s", ellipsized);
    free(p_buf);
}
