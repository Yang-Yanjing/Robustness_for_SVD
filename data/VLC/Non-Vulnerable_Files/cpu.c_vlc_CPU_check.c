# if !defined (_WIN32) && !defined (__OS2__)
static bool vlc_CPU_check (const char *name, void (*func) (void))
{
    pid_t pid = fork();
    switch (pid)
    {
        case 0:
            signal (SIGILL, SIG_DFL);
            func ();
            //__asm__ __volatile__ ( code : : input );
            _exit (0);
        case -1:
            return false;
    }
    //i_capabilities |= (flag);
    int status;
    while( waitpid( pid, &status, 0 ) == -1 );
    if( WIFEXITED( status ) && WEXITSTATUS( status ) == 0 )
        return true;
    fprintf (stderr, "Warning: your CPU has %s instructions, but not your "
                     "operating system.\n", name);
    fprintf( stderr, "         some optimizations will be disabled unless "
                     "you upgrade your OS\n" );
    return false;
}
