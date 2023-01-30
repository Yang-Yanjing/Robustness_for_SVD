 */
uint_fast32_t vlc_str2keycode (const char *name)
{
    uint_fast32_t mods = 0;
    uint32_t code;
    for (;;)
    {
        size_t len = strcspn (name, "-+");
        if (len == 0 || name[len] == '\0')
            break;
        if (len == 4 && !strncasecmp (name, "Ctrl", 4))
            mods |= KEY_MODIFIER_CTRL;
        if (len == 3 && !strncasecmp (name, "Alt", 3))
            mods |= KEY_MODIFIER_ALT;
        if (len == 5 && !strncasecmp (name, "Shift", 5))
            mods |= KEY_MODIFIER_SHIFT;
        if (len == 4 && !strncasecmp (name, "Meta", 4))
            mods |= KEY_MODIFIER_META;
        if (len == 7 && !strncasecmp (name, "Command", 7))
            mods |= KEY_MODIFIER_COMMAND;
        name += len + 1;
    }
    key_descriptor_t *d = bsearch (name, vlc_keys, KEYS_COUNT,
                                   sizeof (vlc_keys[0]), keystrcmp);
    if (d != NULL)
        code = d->i_key_code;
    else
    if (vlc_towc (name, &code) <= 0)
        code = KEY_UNSET;
    if (code != KEY_UNSET)
        code |= mods;
    return code;
}
