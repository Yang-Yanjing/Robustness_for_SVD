}
static void Redraw(intf_thread_t *intf)
{
    intf_sys_t *sys   = intf->p_sys;
    int         box     = sys->box_type;
    int         y       = DrawStatus(intf);
    sys->box_height = LINES - y - 2;
    DrawBox(y++, sys->box_height, sys->color, _(box_title[box]));
    sys->box_y = y;
    if (box != BOX_NONE) {
        FillBox(intf);
        if (sys->box_lines_total == 0)
            sys->box_start = 0;
        else if (sys->box_start > sys->box_lines_total - 1)
            sys->box_start = sys->box_lines_total - 1;
        y += __MIN(sys->box_lines_total - sys->box_start,
                   sys->box_height);
    }
    while (y < LINES - 1)
        DrawEmptyLine(y++, 1, COLS - 2);
    refresh();
}
