}
int main (int argc, char *argv[])
{
#ifdef WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
    static const struct option opts[] =
    {
        { "force",      no_argument,       NULL, 'f' },
        { "help",       no_argument,       NULL, 'h' },
        { "version",    no_argument,       NULL, 'V' },
        { NULL,         no_argument,       NULL, '\0'}
    };
    int c;
    bool force = false;
    while ((c = getopt_long (argc, argv, "fhV", opts, NULL)) != -1)
        switch (c)
        {
            case 'f':
                force = true;
                break;
            case 'h':
                usage (argv[0]);
                return 0;
            case 'V':
                version ();
                return 0;
            default:
                usage (argv[0]);
                return 1;
        }
    for (int i = optind; i < argc; i++)
    {
        const char *path = argv[i];
        if (setenv ("VLC_PLUGIN_PATH", path, 1))
            abort ();
        const char *vlc_argv[4];
        int vlc_argc = 0;
        vlc_argv[vlc_argc++] = "--quiet";
#ifndef __APPLE__
        if (force)
#endif
            vlc_argv[vlc_argc++] = "--reset-plugins-cache";
        vlc_argv[vlc_argc++] = "--"; /* end of options */
        vlc_argv[vlc_argc] = NULL;
        libvlc_instance_t *vlc = libvlc_new (vlc_argc, vlc_argv);
        if (vlc != NULL)
            libvlc_release (vlc);
        if (vlc == NULL)
            fprintf (stderr, "No plugins in %s\n", path);
        if (vlc == NULL)
            return 1;
    }
    return 0;
}
