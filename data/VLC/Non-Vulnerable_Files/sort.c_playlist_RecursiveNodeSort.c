 */
int playlist_RecursiveNodeSort( playlist_t *p_playlist, playlist_item_t *p_node,
                                int i_mode, int i_type )
{
    /* Ask the playlist to reset as we are changing the order */
    pl_priv(p_playlist)->b_reset_currently_playing = true;
    /* Do the real job recursively */
    return recursiveNodeSort(p_playlist,p_node,find_sorting_fn(i_mode,i_type));
}
