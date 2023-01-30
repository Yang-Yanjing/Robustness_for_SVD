 ****************************************************************************/
static void start_color_and_pairs(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    if (!has_colors()) {
        sys->color = false;
        msg_Warn(intf, "Terminal doesn't support colors");
        return;
    }
    start_color();
    for (int i = C_DEFAULT + 1; i < C_MAX; i++)
        init_pair(i, color_pairs[i].f, color_pairs[i].b);
    /* untested, in all my terminals, !can_change_color() --funman */
    if (can_change_color()) {
        color_content(COLOR_YELLOW, &sys->yellow_r, &sys->yellow_g, &sys->yellow_b);
        init_color(COLOR_YELLOW, 960, 500, 0); /* YELLOW -> ORANGE */
    }
}
