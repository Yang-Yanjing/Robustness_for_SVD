}
static int RateCallback( vlc_object_t *p_this, char const *psz_cmd,
                         vlc_value_t oldval, vlc_value_t newval, void *p )
{
    (void)psz_cmd; (void)oldval;(void)p;
    playlist_t *p_playlist = (playlist_t*)p_this;
    PL_LOCK;
    if( pl_priv(p_playlist)->p_input == NULL )
    {
        PL_UNLOCK;
        return VLC_SUCCESS;
    }
    var_SetFloat( pl_priv( p_playlist )->p_input, "rate", newval.f_float );
    PL_UNLOCK;
    return VLC_SUCCESS;
}
