}
static int List( addons_finder_t *p_finder )
{
    addon_type_t types[] = {
        ADDON_EXTENSION,
        ADDON_PLAYLIST_PARSER,
        ADDON_SERVICE_DISCOVERY,
        ADDON_INTERFACE,
        ADDON_META,
    };
    unsigned int i_type = 0;
    LoadCatalog( p_finder );
    /* Browse dirs to find rogue files */
    while( i_type < ARRAY_SIZE( types ) )
    {
        ListScripts( p_finder, types[i_type++] );
    }
    ListSkins( p_finder );
    return VLC_SUCCESS;
}
