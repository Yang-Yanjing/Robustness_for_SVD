 */
int vlc_object_waitpipe( vlc_object_t *obj )
{
    vlc_object_internals_t *internals = vlc_internals( obj );
    vlc_mutex_lock (&pipe_lock);
    if (internals->pipes[0] == -1)
    {
        /* This can only ever happen if someone killed us without locking: */
        assert (internals->pipes[1] == -1);
        /* pipe() is not a cancellation point, but write() is and eventfd() is
         * unspecified (not in POSIX). */
        int canc = vlc_savecancel ();
#if defined (HAVE_SYS_EVENTFD_H)
        internals->pipes[0] = internals->pipes[1] = eventfd (0, EFD_CLOEXEC);
        if (internals->pipes[0] == -1)
#endif
        {
            if (vlc_pipe (internals->pipes))
                internals->pipes[0] = internals->pipes[1] = -1;
        }
        if (internals->pipes[0] != -1 && !atomic_load (&internals->alive))
        {   /* Race condition: vlc_object_kill() already invoked! */
            msg_Dbg (obj, "waitpipe: object already dying");
            write (internals->pipes[1], &(uint64_t){ 1 }, sizeof (uint64_t));
        }
        vlc_restorecancel (canc);
    }
    vlc_mutex_unlock (&pipe_lock);
    return internals->pipes[0];
}
