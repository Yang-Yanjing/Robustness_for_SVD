 */
void vlc_DeinitActions (libvlc_int_t *libvlc, struct vlc_actions *as)
{
    if (unlikely(as == NULL))
        return;
    var_DelCallback (libvlc, "global-key-pressed", vlc_key_to_action,
                     &as->global_map);
    var_DelCallback (libvlc, "key-pressed", vlc_key_to_action, &as->map);
    tdestroy (as->global_map, free);
    tdestroy (as->map, free);
    free (as);
    libvlc->p_hotkeys = NULL;
}
