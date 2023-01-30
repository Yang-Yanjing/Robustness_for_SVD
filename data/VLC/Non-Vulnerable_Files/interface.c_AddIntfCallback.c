/* Following functions are local */
static int AddIntfCallback( vlc_object_t *obj, char const *var,
                            vlc_value_t old, vlc_value_t cur, void *data )
{
    playlist_t *playlist = data;
    int ret = intf_Create( playlist, cur.psz_string );
    if( ret )
        msg_Err( obj, "interface \"%s\" initialization failed",
                 cur.psz_string );
    (void) var; (void) old;
    return ret;
}
