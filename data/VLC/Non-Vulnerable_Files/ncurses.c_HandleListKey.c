}
static bool HandleListKey(intf_thread_t *intf, int key)
{
    intf_sys_t *sys = intf->p_sys;
    playlist_t *p_playlist = pl_Get(intf);
    switch(key)
    {
#ifdef __FreeBSD__
/* workaround for FreeBSD + xterm:
 * see http://www.nabble.com/curses-vs.-xterm-key-mismatch-t3574377.html */
    case KEY_SELECT:
#endif
    case KEY_END:  sys->box_idx = sys->box_lines_total - 1; break;
    case KEY_HOME: sys->box_idx = 0;                            break;
    case KEY_UP:   sys->box_idx--;                              break;
    case KEY_DOWN: sys->box_idx++;                              break;
    case KEY_PPAGE:sys->box_idx -= sys->box_height;         break;
    case KEY_NPAGE:sys->box_idx += sys->box_height;         break;
    default:
        return false;
    }
    CheckIdx(sys);
    if (sys->box_type == BOX_PLAYLIST) {
        PL_LOCK;
        sys->plidx_follow = IsIndex(sys, p_playlist, sys->box_idx);
        PL_UNLOCK;
    }
    return true;
}
