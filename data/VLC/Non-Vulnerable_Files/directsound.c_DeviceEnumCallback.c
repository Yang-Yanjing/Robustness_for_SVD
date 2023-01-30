} ds_list_t;
static int CALLBACK DeviceEnumCallback( LPGUID guid, LPCWSTR desc,
                                        LPCWSTR mod, LPVOID data )
{
    ds_list_t *list = data;
    OLECHAR buf[48];
    if( StringFromGUID2( guid, buf, 48 ) <= 0 )
        return true;
    list->count++;
    list->ids = xrealloc( list->ids, list->count * sizeof(char *) );
    list->names = xrealloc( list->names, list->count * sizeof(char *) );
    list->ids[list->count - 1] = FromWide( buf );
    list->names[list->count - 1] = FromWide( desc );
    if( list->ids == NULL || list->names == NULL )
        abort();
    (void) mod;
    return true;
}
