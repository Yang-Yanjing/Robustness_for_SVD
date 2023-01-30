}
static uint32_t GetTigerColor( decoder_t *p_dec, const char *psz_prefix )
{
    char *psz_tmp;
    uint32_t i_color = 0;
    if( asprintf( &psz_tmp, "%s-color", psz_prefix ) >= 0 )
    {
        uint32_t i_rgb = var_CreateGetInteger( p_dec, psz_tmp );
        var_Destroy( p_dec, psz_tmp );
        free( psz_tmp );
        i_color |= i_rgb;
    }
    if( asprintf( &psz_tmp, "%s-alpha", psz_prefix ) >= 0 )
    {
        uint32_t i_alpha = var_CreateGetInteger( p_dec, psz_tmp );
        var_Destroy( p_dec, psz_tmp );
        free( psz_tmp );
        i_color |= (i_alpha << 24);
    }
    return i_color;
}
