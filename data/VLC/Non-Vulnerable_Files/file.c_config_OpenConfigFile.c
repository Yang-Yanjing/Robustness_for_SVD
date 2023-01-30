}
static FILE *config_OpenConfigFile( vlc_object_t *p_obj )
{
    char *psz_filename = config_GetConfigFile( p_obj );
    if( psz_filename == NULL )
        return NULL;
    msg_Dbg( p_obj, "opening config file (%s)", psz_filename );
    FILE *p_stream = vlc_fopen( psz_filename, "rt" );
    if( p_stream == NULL && errno != ENOENT )
    {
        msg_Err( p_obj, "cannot open config file (%s): %s",
                 psz_filename, vlc_strerror_c(errno) );
    }
#if !( defined(_WIN32) || defined(__APPLE__) || defined(__OS2__) )
    else if( p_stream == NULL && errno == ENOENT )
    {
        /* This is the fallback for pre XDG Base Directory
         * Specification configs */
        char *home = config_GetUserDir(VLC_HOME_DIR);
        char *psz_old;
        if( home != NULL
         && asprintf( &psz_old, "%s/.vlc/" CONFIG_FILE,
                      home ) != -1 )
        {
            p_stream = vlc_fopen( psz_old, "rt" );
            if( p_stream )
            {
                /* Old config file found. We want to write it at the
                 * new location now. */
                msg_Info( p_obj->p_libvlc, "Found old config file at %s. "
                          "VLC will now use %s.", psz_old, psz_filename );
                char *psz_readme;
                if( asprintf(&psz_readme,"%s/.vlc/README",
                             home ) != -1 )
                {
                    FILE *p_readme = vlc_fopen( psz_readme, "wt" );
                    if( p_readme )
                    {
                        fprintf( p_readme, "The VLC media player "
                                 "configuration folder has moved to comply\n"
                                 "with the XDG Base Directory Specification "
                                 "version 0.6. Your\nconfiguration has been "
                                 "copied to the new location:\n%s\nYou can "
                                 "delete this directory and all its contents.",
                                  psz_filename);
                        fclose( p_readme );
                    }
                    free( psz_readme );
                }
                /* Remove the old configuration file so that --reset-config
                 * can work properly. Fortunately, Linux allows removing
                 * open files - with most filesystems. */
                unlink( psz_old );
            }
            free( psz_old );
        }
        free( home );
    }
#endif
    free( psz_filename );
    return p_stream;
}
