/* gst_init( ) is not thread-safe, hence a thread-safe wrapper */
static void vlc_gst_init( void )
{
    static vlc_mutex_t init_lock = VLC_STATIC_MUTEX;
    vlc_mutex_lock( &init_lock );
    gst_init( NULL, NULL );
    vlc_mutex_unlock( &init_lock );
}
