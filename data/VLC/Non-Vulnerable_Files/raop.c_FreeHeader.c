}
static void FreeHeader( void *p_value, void *p_data )
{
    VLC_UNUSED( p_data );
    free( p_value );
}
