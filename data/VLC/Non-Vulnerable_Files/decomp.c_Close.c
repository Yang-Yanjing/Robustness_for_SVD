 */
static void Close (vlc_object_t *obj)
{
    stream_t *stream = (stream_t *)obj;
    stream_sys_t *p_sys = stream->p_sys;
    int status;
    vlc_cancel (p_sys->thread);
    close (p_sys->read_fd);
    vlc_join (p_sys->thread, NULL);
    if (p_sys->write_fd != -1)
        /* Killed before EOF? */
        close (p_sys->write_fd);
    msg_Dbg (obj, "waiting for PID %u", (unsigned)p_sys->pid);
    while (waitpid (p_sys->pid, &status, 0) == -1);
    msg_Dbg (obj, "exit status %d", status);
    if (p_sys->peeked)
        block_Release (p_sys->peeked);
    vlc_mutex_destroy (&p_sys->lock);
    vlc_cond_destroy (&p_sys->wait);
    free (p_sys);
}
