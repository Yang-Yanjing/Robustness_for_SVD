}
static int DenoiseCallback( vlc_object_t *p_this, char const *psz_var,
                            vlc_value_t oldval, vlc_value_t newval,
                            void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    filter_sys_t *sys = (filter_sys_t*)p_data;
    /* Just take values and flag for recalc so we don't block UI thread calling this
     * and don't right thread safety calcing coefs in here without mutex*/
    vlc_mutex_lock( &sys->coefs_mutex );
    if( !strcmp( psz_var, FILTER_PREFIX "luma-spat") )
        sys->luma_spat = newval.f_float;
    else if( !strcmp( psz_var, FILTER_PREFIX "luma-temp") )
        sys->luma_temp = newval.f_float;
    else if( !strcmp( psz_var, FILTER_PREFIX "chroma-temp") )
        sys->chroma_spat = newval.f_float;
    else if( !strcmp( psz_var, FILTER_PREFIX "chroma-spat") )
        sys->chroma_temp = newval.f_float;
    sys->b_recalc_coefs = true;
    vlc_mutex_unlock( &sys->coefs_mutex );
    return VLC_SUCCESS;
}
