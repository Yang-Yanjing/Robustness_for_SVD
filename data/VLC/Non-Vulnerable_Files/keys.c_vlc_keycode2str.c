 */
char *vlc_keycode2str (uint_fast32_t code, bool locale)
{
    char *(*tr) (const char *) = locale ? vlc_gettext : nooptext;
    const char *name;
    char *str, buf[5];
    uintptr_t key = code & ~KEY_MODIFIER;
    for (size_t i = 0; i < KEYS_COUNT; i++)
        if (vlc_keys[i].i_key_code == key)
        {
            name = vlc_keys[i].psz_key_string;
            goto found;
        }
    if (utf8_cp (key, buf) == NULL)
        return NULL;
    name = buf;
found:
    if (asprintf (&str, "%s%s%s%s%s%s",
                  (code & KEY_MODIFIER_CTRL) ? tr(N_("Ctrl+")) : "",
                  (code & KEY_MODIFIER_ALT) ? tr(N_("Alt+")) : "",
                  (code & KEY_MODIFIER_SHIFT) ? tr(N_("Shift+")) : "",
                  (code & KEY_MODIFIER_META) ? tr(N_("Meta+")) : "",
                  (code & KEY_MODIFIER_COMMAND) ? tr(N_("Command+")) : "",
                  tr(name)) == -1)
        return NULL;
    return str;
}
