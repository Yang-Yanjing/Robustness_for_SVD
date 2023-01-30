/* following functions are local */
static int TtyInit(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    struct termios new_termios;
    GfxMode(sys->tty);
    /* Set keyboard settings */
    if (tcgetattr(0, &sys->old_termios) == -1) {
        msg_Err(vd, "tcgetattr failed");
    }
    if (tcgetattr(0, &new_termios) == -1) {
        msg_Err(vd, "tcgetattr failed");
    }
    /* new_termios.c_lflag &= ~ (ICANON | ISIG);
    new_termios.c_lflag |= (ECHO | ECHOCTL); */
    new_termios.c_lflag &= ~ (ICANON);
    new_termios.c_lflag &= ~(ECHO | ECHOCTL);
    new_termios.c_iflag = 0;
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSAFLUSH, &new_termios) == -1) {
        msg_Err(vd, "tcsetattr failed");
    }
    ioctl(sys->tty, VT_RELDISP, VT_ACKACQ);
#if 0
    /* Set-up tty signal handler to be aware of tty changes */
    struct sigaction sig_tty;
    memset(&sig_tty, 0, sizeof(sig_tty));
    sig_tty.sa_handler = SwitchDisplay;
    sigemptyset(&sig_tty.sa_mask);
    if (sigaction(SIGUSR1, &sig_tty, &sys->sig_usr1) ||
        sigaction(SIGUSR2, &sig_tty, &sys->sig_usr2)) {
        msg_Err(vd, "cannot set signal handler (%s)", vlc_strerror_c(errno));
        /* FIXME SIGUSR1 could have succeed */
        goto error_signal;
    }
#endif
    /* Set-up tty according to new signal handler */
    if (-1 == ioctl(sys->tty, VT_GETMODE, &sys->vt_mode)) {
        msg_Err(vd, "cannot get terminal mode (%s)", vlc_strerror_c(errno));
        goto error;
    }
    struct vt_mode vt_mode = sys->vt_mode;
    vt_mode.mode   = VT_PROCESS;
    vt_mode.waitv  = 0;
    vt_mode.relsig = SIGUSR1;
    vt_mode.acqsig = SIGUSR2;
    if (-1 == ioctl(sys->tty, VT_SETMODE, &vt_mode)) {
        msg_Err(vd, "cannot set terminal mode (%s)", vlc_strerror_c(errno));
        goto error;
    }
    return VLC_SUCCESS;
error:
#if 0
    sigaction(SIGUSR1, &sys->sig_usr1, NULL);
    sigaction(SIGUSR2, &sys->sig_usr2, NULL);
error_signal:
#endif
    tcsetattr(0, 0, &sys->old_termios);
    TextMode(sys->tty);
    return VLC_EGENERIC;
}
