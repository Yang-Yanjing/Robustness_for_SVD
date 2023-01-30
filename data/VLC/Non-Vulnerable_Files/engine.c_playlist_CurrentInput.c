 */
input_thread_t * playlist_CurrentInput( playlist_t * p_playlist )
{
    input_thread_t * p_input;
    PL_LOCK;
    p_input = pl_priv(p_playlist)->p_input;
    if( p_input ) vlc_object_hold( p_input );
    PL_UNLOCK;
    return p_input;
}
