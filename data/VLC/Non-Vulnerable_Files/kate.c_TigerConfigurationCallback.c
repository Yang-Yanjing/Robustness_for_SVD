}
static int TigerConfigurationCallback( vlc_object_t *p_this, const char *psz_var,
                                       vlc_value_t oldval, vlc_value_t newval,
                                       void *p_data )
{
    size_t i_idx;
    VLC_UNUSED( p_this );
    VLC_UNUSED( oldval );
    VLC_UNUSED( newval );
    VLC_UNUSED( p_data );
    vlc_mutex_lock( &kate_decoder_list_mutex );
    /* Update all existing decoders from the global user prefs */
    for( i_idx = 0; i_idx < kate_decoder_list_size; i_idx++ )
    {
        decoder_t *p_dec = kate_decoder_list[ i_idx ];
        OnConfigurationChanged( p_dec, psz_var, oldval, newval );
    }
    vlc_mutex_unlock( &kate_decoder_list_mutex );
    return VLC_SUCCESS;
}
