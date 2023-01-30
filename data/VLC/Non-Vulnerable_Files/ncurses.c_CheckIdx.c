 *****************************************************************************/
static void CheckIdx(intf_sys_t *sys)
{
    int lines = sys->box_lines_total;
    int height = LINES - sys->box_y - 2;
    if (height > lines - 1)
        height = lines - 1;
    /* make sure the new index is within the box */
    if (sys->box_idx <= 0) {
        sys->box_idx = 0;
        sys->box_start = 0;
    } else if (sys->box_idx >= lines - 1 && lines > 0) {
        sys->box_idx = lines - 1;
        sys->box_start = sys->box_idx - height;
    }
    /* Fix box start (1st line of the box displayed) */
    if (sys->box_idx < sys->box_start ||
        sys->box_idx > height + sys->box_start + 1) {
        sys->box_start = sys->box_idx - height/2;
        if (sys->box_start < 0)
            sys->box_start = 0;
    } else if (sys->box_idx == sys->box_start - 1) {
        sys->box_start--;
    } else if (sys->box_idx == height + sys->box_start + 1) {
        sys->box_start++;
    }
}
