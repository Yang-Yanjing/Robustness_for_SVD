}
static void HandleKey(intf_thread_t *intf)
{
    intf_sys_t *sys = intf->p_sys;
    int key = getch();
    int box = sys->box_type;
    if (key == -1)
        return;
    if (box == BOX_SEARCH || box == BOX_OPEN) {
        HandleEditBoxKey(intf, key, sys->box_type);
        return;
    }
    if (box == BOX_NONE)
        switch(key)
        {
#ifdef __FreeBSD__
        case KEY_SELECT:
#endif
        case KEY_END:   ChangePosition(intf, +.99);   return;
        case KEY_HOME:  ChangePosition(intf, -1.0);   return;
        case KEY_UP:    ChangePosition(intf, +0.05);  return;
        case KEY_DOWN:  ChangePosition(intf, -0.05);  return;
        default:        HandleCommonKey(intf, key);   return;
        }
    if (box == BOX_BROWSE   && HandleBrowseKey(intf, key))
        return;
    if (box == BOX_PLAYLIST && HandlePlaylistKey(intf, key))
        return;
    if (HandleListKey(intf, key))
        return;
    HandleCommonKey(intf, key);
}
