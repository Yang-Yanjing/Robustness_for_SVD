}
static void CatalogAdd( xml_t *p_xml, const char *psz_arg1,
                          const char *psz_arg2, const char *psz_filename )
{
    VLC_UNUSED(p_xml);
    xmlCatalogAdd( (unsigned char*)psz_arg1, (unsigned char*)psz_arg2,
        (unsigned char*)psz_filename );
}
