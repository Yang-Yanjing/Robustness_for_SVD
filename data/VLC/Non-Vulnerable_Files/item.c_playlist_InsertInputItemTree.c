 */
int playlist_InsertInputItemTree (
    playlist_t *p_playlist, playlist_item_t *p_parent,
    input_item_node_t *p_node, int i_pos, bool b_flat )
{
    playlist_item_t *p_first_leaf = NULL;
    return RecursiveAddIntoParent ( p_playlist, p_parent, p_node, i_pos, b_flat, &p_first_leaf );
}
