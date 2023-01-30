VLC_NORETURN
static void *RunThread (void *self)
{
    sap_address_t *addr = self;
    vlc_mutex_lock (&addr->lock);
    mutex_cleanup_push (&addr->lock);
    for (;;)
    {
        sap_session_t *p_session;
        mtime_t deadline;
        while (addr->first == NULL)
            vlc_cond_wait (&addr->wait, &addr->lock);
        assert (addr->session_count > 0);
        deadline = mdate ();
        for (p_session = addr->first; p_session; p_session = p_session->next)
        {
            send (addr->fd, p_session->data, p_session->length, 0);
            deadline += addr->interval * CLOCK_FREQ / addr->session_count;
            if (vlc_cond_timedwait (&addr->wait, &addr->lock, deadline) == 0)
                break; /* list may have changed! */
        }
    }
    vlc_cleanup_pop ();
    assert (0);
}
