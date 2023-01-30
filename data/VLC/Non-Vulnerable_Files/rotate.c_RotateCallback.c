 *****************************************************************************/
static int RotateCallback( vlc_object_t *p_this, char const *psz_var,
                           vlc_value_t oldval, vlc_value_t newval, void *data )
{
    filter_sys_t *p_sys = data;
    store_trigo( p_sys, newval.f_float );
    (void) p_this; (void) psz_var; (void) oldval;
    return VLC_SUCCESS;
}
