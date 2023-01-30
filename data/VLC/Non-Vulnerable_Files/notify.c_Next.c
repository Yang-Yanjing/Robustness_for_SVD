/* libnotify callback, called when the "Next" button is pressed */
static void Next( NotifyNotification *notification, gchar *psz, gpointer p )
{
    vlc_object_t *p_object = (vlc_object_t*)p;
    VLC_UNUSED(psz);
    notify_notification_close( notification, NULL );
    playlist_Next( pl_Get( p_object ) );
}
