}
static void check_items_order_callback(const libvlc_event_t * p_event, void * user_data)
{
    struct check_items_order_data *checks = user_data;
    libvlc_media_t *md = p_event->u.media_list_player_next_item_set.item;
    assert(checks->index < checks->count);
    if (checks->items[checks->index] != md)
    {
        char *title = libvlc_media_get_meta(md, libvlc_meta_Title);
        log ("Got items %s\n", title);
        free(title);
    }
    assert(checks->items[checks->index] == md);
    char *title = libvlc_media_get_meta(md, libvlc_meta_Title);
    log ("Item %d '%s' was correctly queued\n", checks->index, title);
    free(title);
    if (checks->index == (checks->count - 1))
    {
        log ("Done playing with success\n");
        checks->done_playing = true;
    }
    checks->index++;
}
