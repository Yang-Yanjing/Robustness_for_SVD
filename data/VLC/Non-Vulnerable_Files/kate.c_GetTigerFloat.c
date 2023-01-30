}
static double GetTigerFloat( decoder_t *p_dec, const char *psz_name )
{
    double f_value = var_CreateGetFloat( p_dec, psz_name );
    var_Destroy( p_dec, psz_name );
    return f_value;
}
