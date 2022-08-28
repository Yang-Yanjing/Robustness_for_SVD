static void add_renditions_to_variant(HLSContext *c, struct variant *var,
                                      enum AVMediaType type, const char *group_id)
{
    int i;
    for (i = 0; i < c->n_renditions; i++) {
        struct rendition *rend = c->renditions[i];
        if (rend->type == type && !strcmp(rend->group_id, group_id)) {
            if (rend->playlist)
                

                dynarray_add(&var->playlists, &var->n_playlists, rend->playlist);
            else
                

                dynarray_add(&var->playlists[0]->renditions,
                             &var->playlists[0]->n_renditions,
                             rend);
        }
    }
}
