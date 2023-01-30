}
static void PlaylistAddNode(intf_sys_t *sys, playlist_item_t *node,
                            const char *c)
{
    for (int k = 0; k < node->i_children; k++) {
        bool last = k == node->i_children - 1;
        playlist_item_t *p_child = node->pp_children[k];
        if (!PlaylistAddChild(sys, p_child, c, last ? '`' : '|'))
            return;
        if (p_child->i_children <= 0)
            continue;
        if (*c) {
            char *tmp;
            if (asprintf(&tmp, "%s%c ", c, last ? ' ' : '|') == -1)
                return;
            PlaylistAddNode(sys, p_child, tmp);
            free(tmp);
        } else {
            PlaylistAddNode(sys, p_child, " ");
        }
    }
}
