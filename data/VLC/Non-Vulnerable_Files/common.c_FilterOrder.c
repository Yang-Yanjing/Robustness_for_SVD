/* Return the order in which filters should be inserted */
static int FilterOrder( const char *psz_name )
{
    static const struct {
        const char psz_name[10];
        int        i_order;
    } filter[] = {
        { "equalizer",  0 },
    };
    for( unsigned i = 0; i < ARRAY_SIZE(filter); i++ )
    {
        if( !strcmp( filter[i].psz_name, psz_name ) )
            return filter[i].i_order;
    }
    return INT_MAX;
}
