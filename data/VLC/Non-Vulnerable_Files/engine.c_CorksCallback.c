 */
static int CorksCallback( vlc_object_t *obj, char const *var,
                          vlc_value_t old, vlc_value_t cur, void *dummy )
{
    playlist_t *pl = (playlist_t *)obj;
    msg_Dbg( obj, "corks count: %"PRId64" -> %"PRId64, old.i_int, cur.i_int );
    if( !old.i_int == !cur.i_int )
        return VLC_SUCCESS; /* nothing to do */
    if( !var_InheritBool( obj, "playlist-cork" ) )
        return VLC_SUCCESS;
    input_thread_t *input = playlist_CurrentInput(pl);
    if( input == NULL )
        return VLC_SUCCESS;
    bool playing = var_GetInteger(input, "state") == PLAYING_S;
    vlc_object_release(input);
    if( playing == !cur.i_int )
        return VLC_SUCCESS;
    msg_Dbg( obj, "%scorked", cur.i_int ? "" : "un" );
    playlist_Pause( pl );
    (void) var; (void) dummy;
    return VLC_SUCCESS;
}
