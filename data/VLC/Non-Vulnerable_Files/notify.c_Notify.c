}
static int Notify( vlc_object_t *p_this, const char *psz_temp, GdkPixbuf *pix,
                   intf_thread_t *p_intf )
{
    intf_sys_t *p_sys = p_intf->p_sys;
    NotifyNotification * notification;
    /* Close previous notification if still active */
    if( p_sys->notification )
    {
        GError *p_error = NULL;
        notify_notification_close( p_sys->notification, &p_error );
        g_object_unref( p_sys->notification );
    }
    notification = notify_notification_new( _("Now Playing"),
                                            psz_temp, NULL
#if NOTIFY_CHECK_VERSION (0, 7, 0)
                                          );
#else
                                          , NULL );
#endif
    notify_notification_set_timeout( notification,
                                var_InheritInteger(p_this, "notify-timeout") );
    notify_notification_set_urgency( notification, NOTIFY_URGENCY_LOW );
    if( pix )
    {
        notify_notification_set_icon_from_pixbuf( notification, pix );
        gdk_pixbuf_unref( pix );
    }
    /* Adds previous and next buttons in the notification if actions are supported. */
    if( p_sys->b_has_actions )
    {
      notify_notification_add_action( notification, "previous", _("Previous"), Prev,
                                      (gpointer*)p_intf, NULL );
      notify_notification_add_action( notification, "next", _("Next"), Next,
                                      (gpointer*)p_intf, NULL );
    }
    notify_notification_show( notification, NULL);
    /* Stores the notification to be able to close it */
    p_sys->notification = notification;
    return VLC_SUCCESS;
}
