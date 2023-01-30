}
static int Intf( vlc_object_t *p_this, char const *psz_cmd,
                 vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    intf_thread_t *intf = (intf_thread_t *)p_this;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    return intf_Create(pl_Get(intf), newval.psz_string );
}
