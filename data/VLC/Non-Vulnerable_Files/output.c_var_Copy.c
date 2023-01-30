static void aout_Destructor( vlc_object_t * p_this );
static int var_Copy (vlc_object_t *src, const char *name, vlc_value_t prev,
                     vlc_value_t value, void *data)
{
    vlc_object_t *dst = data;
    (void) src; (void) prev;
    return var_Set (dst, name, value);
}
