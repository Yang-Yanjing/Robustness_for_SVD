 */
int playlist_Add( playlist_t *p_playlist, const char *psz_uri,
                  const char *psz_name, int i_mode, int i_pos,
                  bool b_playlist, bool b_locked )
{
    return playlist_AddExt( p_playlist, psz_uri, psz_name,
                            i_mode, i_pos, -1, 0, NULL, 0, b_playlist, b_locked );
}
