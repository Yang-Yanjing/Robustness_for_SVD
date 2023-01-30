}
static int PPNameCallback( vlc_object_t *p_this, const char *psz_var,
                           vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(psz_var); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    filter_t *p_filter = (filter_t *)p_this;
    int i_quality = var_GetInteger( p_filter, FILTER_PREFIX "q" );
    PPChangeMode( p_filter, *newval.psz_string ? newval.psz_string : NULL,
                  i_quality );
    return VLC_SUCCESS;
}
