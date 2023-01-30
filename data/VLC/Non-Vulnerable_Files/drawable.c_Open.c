 */
static int Open (vout_window_t *wnd, const vout_window_cfg_t *cfg)
{
    VLC_UNUSED (cfg);
    uintptr_t val = var_InheritInteger (wnd, "drawable-hwnd");
    if (val == 0)
        return VLC_EGENERIC;
    uintptr_t *tab;
    size_t n = 0;
    vlc_mutex_lock (&serializer);
    if (used != NULL)
        for (/*n = 0*/; used[n]; n++)
            if (used[n] == val)
            {
                msg_Warn (wnd, "HWND 0x%zX is busy", val);
                val = 0;
                goto skip;
            }
    tab = realloc (used, sizeof (*used) * (n + 2));
    if (likely(tab != NULL))
    {
        used = tab;
        used[n] = val;
        used[n + 1] = 0;
    }
    else
        val = 0;
skip:
    vlc_mutex_unlock (&serializer);
    if (val == 0)
        return VLC_EGENERIC;
    wnd->handle.hwnd = (void *)val;
    wnd->control = Control;
    wnd->sys = (void *)val;
    return VLC_SUCCESS;
}
