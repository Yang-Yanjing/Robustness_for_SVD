 *****************************************************************************/
static void AddToQueue (intf_thread_t *p_this)
{
    mtime_t                     played_time;
    intf_sys_t                  *p_sys = p_this->p_sys;
    vlc_mutex_lock(&p_sys->lock);
    if (!p_sys->b_submit)
        goto end;
    /* wait for the user to listen enough before submitting */
    played_time = mdate() - p_sys->p_current_song.i_start -
                            p_sys->time_total_pauses;
    played_time /= 1000000; /* µs → s */
    /*HACK: it seam that the preparsing sometime fail,
            so use the playing time as the song length */
    if (p_sys->p_current_song.i_l == 0)
        p_sys->p_current_song.i_l = played_time;
    /* Don't send song shorter than 30s */
    if (p_sys->p_current_song.i_l < 30)
    {
        msg_Dbg(p_this, "Song too short (< 30s), not submitting");
        goto end;
    }
    /* Send if the user had listen more than 240s OR half the track length */
    if ((played_time < 240) &&
        (played_time < (p_sys->p_current_song.i_l / 2)))
    {
        msg_Dbg(p_this, "Song not listened long enough, not submitting");
        goto end;
    }
    /* Check that all meta are present */
    if (!p_sys->p_current_song.psz_a || !*p_sys->p_current_song.psz_a ||
        !p_sys->p_current_song.psz_t || !*p_sys->p_current_song.psz_t)
    {
        msg_Dbg(p_this, "Missing artist or title, not submitting");
        goto end;
    }
    if (p_sys->i_songs >= QUEUE_MAX)
    {
        msg_Warn(p_this, "Submission queue is full, not submitting");
        goto end;
    }
    msg_Dbg(p_this, "Song will be submitted.");
#define QUEUE_COPY(a) \
    p_sys->p_queue[p_sys->i_songs].a = p_sys->p_current_song.a
#define QUEUE_COPY_NULL(a) \
    QUEUE_COPY(a); \
    p_sys->p_current_song.a = NULL
    QUEUE_COPY(i_l);
    QUEUE_COPY_NULL(psz_n);
    QUEUE_COPY_NULL(psz_a);
    QUEUE_COPY_NULL(psz_t);
    QUEUE_COPY_NULL(psz_b);
    QUEUE_COPY_NULL(psz_m);
    QUEUE_COPY(date);
#undef QUEUE_COPY_NULL
#undef QUEUE_COPY
    p_sys->i_songs++;
    /* signal the main loop we have something to submit */
    vlc_cond_signal(&p_sys->wait);
end:
    DeleteSong(&p_sys->p_current_song);
    p_sys->b_submit = false;
    vlc_mutex_unlock(&p_sys->lock);
}
