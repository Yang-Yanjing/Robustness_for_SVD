}
static void TtyExit(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    /* Reset the terminal */
    ioctl(sys->tty, VT_SETMODE, &sys->vt_mode);
#if 0
    /* Remove signal handlers */
    sigaction(SIGUSR1, &sys->sig_usr1, NULL);
    sigaction(SIGUSR2, &sys->sig_usr2, NULL);
#endif
    /* Reset the keyboard state */
    tcsetattr(0, 0, &sys->old_termios);
    /* Return to text mode */
    TextMode(sys->tty);
}
