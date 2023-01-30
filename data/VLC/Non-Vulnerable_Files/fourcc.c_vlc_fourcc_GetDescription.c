/* */
const char *vlc_fourcc_GetDescription( int i_cat, vlc_fourcc_t i_fourcc )
{
    entry_t e = Find( i_cat, i_fourcc );
    return e.psz_description;
}
