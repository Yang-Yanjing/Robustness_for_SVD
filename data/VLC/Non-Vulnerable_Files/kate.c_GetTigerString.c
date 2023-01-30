}
static char *GetTigerString( decoder_t *p_dec, const char *psz_name )
{
    char *psz_value = var_CreateGetString( p_dec, psz_name );
    if( psz_value)
    {
        psz_value = strdup( psz_value );
    }
    var_Destroy( p_dec, psz_name );
    return psz_value;
}
