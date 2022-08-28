void term_init(void)
{
#if HAVE_TERMIOS_H
    if (!run_as_daemon && stdin_interaction) {
        struct termios tty;
        if (tcgetattr (0, &tty) == 0) {
            oldtty = tty;
            restore_tty = 1;
            tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                             |INLCR|IGNCR|ICRNL|IXON);
            tty.c_oflag |= OPOST;
            tty.c_lflag &= ~(ECHO|ECHONL|ICANON|IEXTEN);
            tty.c_cflag &= ~(CSIZE|PARENB);
            tty.c_cflag |= CS8;
            tty.c_cc[VMIN] = 1;
            tty.c_cc[VTIME] = 0;
            tcsetattr (0, TCSANOW, &tty);
        }
        signal(SIGQUIT, sigterm_handler); 
    }
#endif
    signal(SIGINT , sigterm_handler); 
    signal(SIGTERM, sigterm_handler); 
#ifdef SIGXCPU
    signal(SIGXCPU, sigterm_handler);
#endif
#if HAVE_SETCONSOLECTRLHANDLER
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);
#endif
}
