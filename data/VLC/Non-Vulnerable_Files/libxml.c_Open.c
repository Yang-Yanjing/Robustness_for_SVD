 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    xml_t *p_xml = (xml_t *)p_this;
    if( !xmlHasFeature( XML_WITH_THREAD ) )
        return VLC_EGENERIC;
    vlc_mutex_lock( &lock );
    xmlInitParser();
    vlc_mutex_unlock( &lock );
    p_xml->pf_catalog_load = CatalogLoad;
    p_xml->pf_catalog_add  = CatalogAdd;
    return VLC_SUCCESS;
}
