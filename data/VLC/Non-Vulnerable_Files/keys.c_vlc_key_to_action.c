};
static int vlc_key_to_action (vlc_object_t *obj, const char *varname,
                              vlc_value_t prevkey, vlc_value_t curkey, void *d)
{
    void *const *map = d;
    const struct mapping **pent;
    uint32_t keycode = curkey.i_int;
    pent = tfind (&keycode, map, keycmp);
    if (pent == NULL)
        return VLC_SUCCESS;
    (void) varname;
    (void) prevkey;
    return var_SetInteger (obj, "key-action", (*pent)->action);
}
