}
static int GetTigerInteger( decoder_t *p_dec, const char *psz_name )
{
    int i_value = var_CreateGetInteger( p_dec, psz_name );
    var_Destroy( p_dec, psz_name );
    return i_value;
}
