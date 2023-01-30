}
void formatSnapshotItem( input_item_t *p_item )
{
    if( !p_item )
        return;
    char* psz_uri = input_item_GetURI( p_item );
    /* copy the snapshot mrl as a ArtURL */
    if( psz_uri )
        input_item_SetArtURL( p_item, psz_uri );
    free( psz_uri );
}
