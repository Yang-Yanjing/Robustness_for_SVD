 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_this;
    intf_sys_t *p_sys = p_intf->p_sys;
    /* Flush the queue and unsubscribe from the message queue */
    vlc_LogSet( p_intf->p_libvlc, NULL, NULL );
    /* Close the log file */
#ifdef HAVE_SYSLOG_H
    if( p_sys->p_file == NULL )
    {
        closelog();
        free( p_sys->ident );
    }
    else
#endif
    if( p_sys->p_file )
    {
        fputs( p_sys->footer, p_sys->p_file );
        fclose( p_sys->p_file );
    }
    /* Destroy structure */
    free( p_sys );
}
