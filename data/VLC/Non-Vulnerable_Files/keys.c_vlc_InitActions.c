 */
struct vlc_actions *vlc_InitActions (libvlc_int_t *libvlc)
{
    vlc_object_t *obj = VLC_OBJECT(libvlc);
    struct hotkey *keys;
    struct vlc_actions *as = malloc (sizeof (*as) + ACTIONS_COUNT * sizeof (*keys));
    if (unlikely(as == NULL))
        return NULL;
    as->map = NULL;
    as->global_map = NULL;
    keys = as->keys;
    var_Create (obj, "key-pressed", VLC_VAR_INTEGER);
    var_Create (obj, "global-key-pressed", VLC_VAR_INTEGER);
    var_Create (obj, "key-action", VLC_VAR_INTEGER);
    /* Initialize from configuration */
    for (size_t i = 0; i < ACTIONS_COUNT; i++)
    {
#ifndef NDEBUG
        if (i > 0
         && strcmp (actions[i-1].name, actions[i].name) >= 0)
        {
            msg_Err (libvlc, "key-%s and key-%s are not ordered properly",
                     actions[i-1].name, actions[i].name);
            abort ();
        }
#endif
        keys->psz_action = actions[i].name;
        keys++;
        char name[12 + MAXACTION];
        snprintf (name, sizeof (name), "global-key-%s", actions[i].name);
        vlc_InitAction (obj, &as->map, name + 7, actions[i].value);
        vlc_InitAction (obj, &as->global_map, name, actions[i].value);
    }
    keys->psz_action = NULL;
    /* Initialize mouse wheel events */
    int mousemode = var_InheritInteger (obj, "hotkeys-mousewheel-mode");
    if (mousemode < 2)
    {
        vlc_AddMapping (&as->map,
                        mousemode ? KEY_MOUSEWHEELRIGHT : KEY_MOUSEWHEELUP,
                        ACTIONID_VOL_UP);
        vlc_AddMapping (&as->map,
                        mousemode ? KEY_MOUSEWHEELLEFT : KEY_MOUSEWHEELDOWN,
                        ACTIONID_VOL_DOWN);
        vlc_AddMapping (&as->map,
                        mousemode ? KEY_MOUSEWHEELUP : KEY_MOUSEWHEELRIGHT,
                        ACTIONID_JUMP_FORWARD_EXTRASHORT);
        vlc_AddMapping (&as->map,
                        mousemode ? KEY_MOUSEWHEELDOWN : KEY_MOUSEWHEELLEFT,
                        ACTIONID_JUMP_BACKWARD_EXTRASHORT);
    }
    libvlc->p_hotkeys = as->keys;
    var_AddCallback (obj, "key-pressed", vlc_key_to_action, &as->map);
    var_AddCallback (obj, "global-key-pressed", vlc_key_to_action,
                     &as->global_map);
    return as;
}
