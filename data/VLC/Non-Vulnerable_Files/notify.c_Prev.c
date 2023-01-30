/* libnotify callback, called when the "Previous" button is pressed */
static void Prev( NotifyNotification *notification, gchar *psz, gpointer p )
{
    vlc_object_t *p_object = (vlc_object_t*)p;
    VLC_UNUSED(psz);
    notify_notification_close( notification, NULL );
    playlist_Prev( pl_Get( p_object ) );
}
