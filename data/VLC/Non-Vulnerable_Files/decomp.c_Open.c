 */
static int Open (stream_t *stream, const char *path)
{
    stream_sys_t *p_sys = stream->p_sys = malloc (sizeof (*p_sys));
    if (p_sys == NULL)
        return VLC_ENOMEM;
    stream->pf_read = Read;
    stream->pf_peek = Peek;
    stream->pf_control = Control;
    vlc_cond_init (&p_sys->wait);
    vlc_mutex_init (&p_sys->lock);
    p_sys->paused = false;
    p_sys->pid = -1;
    p_sys->offset = 0;
    p_sys->peeked = NULL;
    stream_Control (stream->p_source, STREAM_CAN_PAUSE, &p_sys->can_pause);
    stream_Control (stream->p_source, STREAM_CAN_CONTROL_PACE,
                    &p_sys->can_pace);
    stream_Control (stream->p_source, STREAM_GET_PTS_DELAY, &p_sys->pts_delay);
    /* I am not a big fan of the pyramid style, but I cannot think of anything
     * better here. There are too many failure cases. */
    int ret = VLC_EGENERIC;
    int comp[2];
    /* We use two pipes rather than one stream socket pair, so that we can
     * use vmsplice() on Linux. */
    if (vlc_pipe (comp) == 0)
    {
        p_sys->write_fd = comp[1];
        int uncomp[2];
        if (vlc_pipe (uncomp) == 0)
        {
            p_sys->read_fd = uncomp[0];
#if (_POSIX_SPAWN >= 0)
            posix_spawn_file_actions_t actions;
            if (posix_spawn_file_actions_init (&actions) == 0)
            {
                char *const argv[] = { (char *)path, NULL };
                if (!posix_spawn_file_actions_adddup2 (&actions, comp[0], 0)
                 && !posix_spawn_file_actions_adddup2 (&actions, uncomp[1], 1)
                 && !posix_spawnp (&p_sys->pid, path, &actions, NULL, argv,
                                   environ))
                {
                    if (vlc_clone (&p_sys->thread, Thread, stream,
                                   VLC_THREAD_PRIORITY_INPUT) == 0)
                        ret = VLC_SUCCESS;
                }
                else
                {
                    msg_Err (stream, "cannot execute %s", path);
                    p_sys->pid = -1;
                }
                posix_spawn_file_actions_destroy (&actions);
            }
#else /* _POSIX_SPAWN */
            switch (p_sys->pid = fork ())
            {
                case -1:
                    msg_Err (stream, "cannot fork: %s", vlc_strerror_c(errno));
                    break;
                case 0:
                    dup2 (comp[0], 0);
                    dup2 (uncomp[1], 1);
                    execlp (path, path, (char *)NULL);
                    exit (1); /* if we get, execlp() failed! */
                default:
                    if (vlc_clone (&p_sys->thread, Thread, stream,
                                   VLC_THREAD_PRIORITY_INPUT) == 0)
                        ret = VLC_SUCCESS;
            }
#endif /* _POSIX_SPAWN < 0 */
            close (uncomp[1]);
            if (ret != VLC_SUCCESS)
                close (uncomp[0]);
        }
        close (comp[0]);
        if (ret != VLC_SUCCESS)
            close (comp[1]);
    }
    if (ret == VLC_SUCCESS)
        return VLC_SUCCESS;
    if (p_sys->pid != -1)
        while (waitpid (p_sys->pid, &(int){ 0 }, 0) == -1);
    vlc_mutex_destroy (&p_sys->lock);
    vlc_cond_destroy (&p_sys->wait);
    free (p_sys);
    return ret;
}
