static void
sigterm_handler(int sig)
{
    received_sigterm = sig;
    received_nb_signals++;
    term_exit_sigsafe();
    if(received_nb_signals > 3) {
        write(2, "Received > 3 system signals, hard exiting\n",
                           strlen("Received > 3 system signals, hard exiting\n"));
        exit(123);
    }
}
