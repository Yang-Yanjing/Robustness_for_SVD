 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys;
    CONSOLE_INTRO_MSG;
    msg_Info( p_intf, "using logger." );
    /* Allocate instance and initialize some members */
    p_sys = p_intf->p_sys = (intf_sys_t *)malloc( sizeof( intf_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->p_file = NULL;
    vlc_log_cb cb = TextPrint;
    const char *filename = LOG_FILE_TEXT, *header = TEXT_HEADER;
    p_sys->footer = TEXT_FOOTER;
    char *mode = var_InheritString( p_intf, "logmode" );
    if( mode != NULL )
    {
        if( !strcmp( mode, "html" ) )
        {
            p_sys->footer = HTML_FOOTER;
            filename = LOG_FILE_HTML;
            header = HTML_HEADER;
            cb = HtmlPrint;
        }
#ifdef HAVE_SYSLOG_H
        else if( !strcmp( mode, "syslog" ) )
            cb = SyslogPrint;
#endif
#ifdef __ANDROID__
        else if( !strcmp( mode, "android" ) )
            cb = AndroidPrint;
#endif
        else if( strcmp( mode, "text" ) )
            msg_Warn( p_intf, "invalid log mode `%s', using `text'", mode );
        free( mode );
    }
#ifdef HAVE_SYSLOG_H
    if( cb == SyslogPrint )
    {
        int i_facility;
        char *psz_facility = var_InheritString( p_intf, "syslog-facility" );
        if( psz_facility )
        {
            bool b_valid = 0;
            for( size_t i = 0; i < fac_entries; ++i )
            {
                if( !strcmp( psz_facility, fac_name[i] ) )
                {
                    i_facility = fac_number[i];
                    b_valid = 1;
                    break;
                }
            }
            if( !b_valid )
            {
                msg_Warn( p_intf, "invalid syslog facility `%s', using `%s'",
                          psz_facility, fac_name[0] );
                i_facility = fac_number[0];
            }
            free( psz_facility );
        }
        else
        {
            msg_Warn( p_intf, "no syslog facility specified, using `%s'",
                      fac_name[0] );
            i_facility = fac_number[0];
        }
        char *psz_syslog_ident = var_InheritString( p_intf, "syslog-ident" );
        if (unlikely(psz_syslog_ident == NULL))
        {
            free( p_sys );
            return VLC_ENOMEM;
        }
        p_sys->ident = psz_syslog_ident;
        openlog( p_sys->ident, LOG_PID|LOG_NDELAY, i_facility );
        p_sys->p_file = NULL;
    }
    else
#endif
#ifdef __ANDROID__
    if( cb == AndroidPrint )
    {
        /* nothing to do */
    }
    else
#endif
    {
        char *psz_file = var_InheritString( p_intf, "logfile" );
        if( !psz_file )
        {
#ifdef __APPLE__
# define LOG_DIR "Library/Logs"
            char *home = config_GetUserDir(VLC_HOME_DIR);
            if( home == NULL
             || asprintf( &psz_file, "%s/"LOG_DIR"/%s", home,
                          filename ) == -1 )
                psz_file = NULL;
            free(home);
            filename = psz_file;
#endif
            msg_Warn( p_intf, "no log filename provided, using `%s'",
                      filename );
        }
        else
            filename = psz_file;
        /* Open the log file and remove any buffering for the stream */
        msg_Dbg( p_intf, "opening logfile `%s'", filename );
        p_sys->p_file = vlc_fopen( filename, "at" );
        if( p_sys->p_file == NULL )
        {
            msg_Err( p_intf, "error opening logfile `%s': %s", filename,
                     vlc_strerror_c(errno) );
            free( psz_file );
            free( p_sys );
            return VLC_EGENERIC;
        }
        free( psz_file );
        setvbuf( p_sys->p_file, NULL, _IONBF, 0 );
        fputs( header, p_sys->p_file );
    }
    vlc_LogSet( p_intf->p_libvlc, cb, p_intf );
    return VLC_SUCCESS;
}
