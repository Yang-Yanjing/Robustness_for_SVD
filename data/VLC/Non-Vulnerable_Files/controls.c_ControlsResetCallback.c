}
static int ControlsResetCallback (vlc_object_t *obj, const char *var,
                                  vlc_value_t old, vlc_value_t cur, void *data)
{
    ControlsReset (obj, data);
    (void) var; (void) old; (void) cur;
    return VLC_SUCCESS;
}
