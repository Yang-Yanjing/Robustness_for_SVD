};
static int ConvertKey( USHORT i_pmkey )
{
    int i;
    for( i = 0; pmkeys_to_vlckeys[ i ].i_pmkey != 0; i++ )
    {
        if( pmkeys_to_vlckeys[ i ].i_pmkey == i_pmkey )
            return pmkeys_to_vlckeys[ i ].i_vlckey;
    }
    return 0;
}
