}
static void FindIndex(intf_sys_t *sys, playlist_t *p_playlist)
{
    int plidx = sys->box_idx;
    int max = sys->plist_entries;
    PL_LOCK;
    if (!IsIndex(sys, p_playlist, plidx))
        for (int i = 0; i < max; i++)
            if (IsIndex(sys, p_playlist, i)) {
                sys->box_idx = i;
                CheckIdx(sys);
                break;
            }
    PL_UNLOCK;
    sys->plidx_follow = true;
}
