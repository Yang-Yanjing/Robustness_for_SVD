 */
void playlist_Clear( playlist_t * p_playlist, bool b_locked )
{
    PL_LOCK_IF( !b_locked );
    playlist_NodeEmpty( p_playlist, p_playlist->p_playing, true );
    PL_UNLOCK_IF( !b_locked );
}
