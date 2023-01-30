 */
static void Close (vout_window_t *wnd)
{
    uintptr_t val = (uintptr_t)wnd->sys;
    size_t n = 0;
    /* Remove this drawable from the list of busy ones */
    vlc_mutex_lock (&serializer);
    assert (used != NULL);
    while (used[n] != val)
    {
        assert (used[n]);
        n++;
    }
    do
        used[n] = used[n + 1];
    while (used[++n] != 0);
    if (n == 0)
    {
         free (used);
         used = NULL;
    }
    vlc_mutex_unlock (&serializer);
}
