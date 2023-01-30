}
static void MainBoxWrite(intf_sys_t *sys, int l, const char *p_fmt, ...)
{
    va_list     vl_args;
    char        *p_buf;
    bool        b_selected = l == sys->box_idx;
    if (l < sys->box_start || l - sys->box_start >= sys->box_height)
        return;
    va_start(vl_args, p_fmt);
    int i_ret = vasprintf(&p_buf, p_fmt, vl_args);
    va_end(vl_args);
    if (i_ret == -1)
        return;
    if (b_selected) attron(A_REVERSE);
    mvnprintw(sys->box_y + l - sys->box_start, 1, COLS - 2, "%s", p_buf);
    if (b_selected) attroff(A_REVERSE);
    free(p_buf);
}
