}
static int PPQCallback( vlc_object_t *p_this, const char *psz_var,
                        vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(psz_var); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    filter_t *p_filter = (filter_t *)p_this;
    char *psz_name = var_GetNonEmptyString( p_filter, FILTER_PREFIX "name" );
    PPChangeMode( p_filter, psz_name, newval.i_int );
    free( psz_name );
    return VLC_SUCCESS;
}
