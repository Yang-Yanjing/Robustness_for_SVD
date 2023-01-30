 */
static void vlc_InitAction (vlc_object_t *obj, void **map,
                            const char *confname, vlc_action_t action)
{
    char *keys = var_InheritString (obj, confname);
    if (keys == NULL)
        return;
    for (char *buf, *key = strtok_r (keys, "\t", &buf);
         key != NULL;
         key = strtok_r (NULL, "\t", &buf))
    {
        uint32_t code = vlc_str2keycode (key);
        if (code == KEY_UNSET)
        {
            msg_Warn (obj, "Key \"%s\" unrecognized", key);
            continue;
        }
        if (vlc_AddMapping (map, code, action) == EEXIST)
            msg_Warn (obj, "Key \"%s\" bound to multiple actions", key);
    }
    free (keys);
}
