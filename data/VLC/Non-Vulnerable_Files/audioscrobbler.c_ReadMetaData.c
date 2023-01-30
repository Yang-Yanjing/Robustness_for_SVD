 *****************************************************************************/
static void ReadMetaData(intf_thread_t *p_this)
{
    intf_sys_t *p_sys = p_this->p_sys;
    input_thread_t *p_input = pl_CurrentInput(p_this);
    if (!p_input)
        return;
    input_item_t *p_item = input_GetItem(p_input);
    if (!p_item)
    {
        vlc_object_release(p_input);
        return;
    }
#define ALLOC_ITEM_META(a, b) do { \
        char *psz_meta = input_item_Get##b(p_item); \
        if (psz_meta && *psz_meta) \
            a = encode_URI_component(psz_meta); \
        free(psz_meta); \
    } while (0)
    vlc_mutex_lock(&p_sys->lock);
    p_sys->b_meta_read = true;
    ALLOC_ITEM_META(p_sys->p_current_song.psz_a, Artist);
    if (!p_sys->p_current_song.psz_a)
    {
        msg_Dbg(p_this, "No artist..");
        DeleteSong(&p_sys->p_current_song);
        goto end;
    }
    ALLOC_ITEM_META(p_sys->p_current_song.psz_t, Title);
    if (!p_sys->p_current_song.psz_t)
    {
        msg_Dbg(p_this, "No track name..");
        DeleteSong(&p_sys->p_current_song);
        goto end;
    }
    /* Now we have read the mandatory meta data, so we can submit that info */
    p_sys->b_submit = true;
    ALLOC_ITEM_META(p_sys->p_current_song.psz_b, Album);
    if (!p_sys->p_current_song.psz_b)
        p_sys->p_current_song.psz_b = calloc(1, 1);
    ALLOC_ITEM_META(p_sys->p_current_song.psz_m, TrackID);
    if (!p_sys->p_current_song.psz_m)
        p_sys->p_current_song.psz_m = calloc(1, 1);
    p_sys->p_current_song.i_l = input_item_GetDuration(p_item) / 1000000;
    ALLOC_ITEM_META(p_sys->p_current_song.psz_n, TrackNum);
    if (!p_sys->p_current_song.psz_n)
        p_sys->p_current_song.psz_n = calloc(1, 1);
#undef ALLOC_ITEM_META
    msg_Dbg(p_this, "Meta data registered");
end:
    vlc_mutex_unlock(&p_sys->lock);
    vlc_object_release(p_input);
}
