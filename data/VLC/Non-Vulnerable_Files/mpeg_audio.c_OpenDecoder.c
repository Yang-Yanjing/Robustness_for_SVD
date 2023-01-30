}
static int OpenDecoder( vlc_object_t *p_this )
{
    /* HACK: Don't use this codec if we don't have an mpga audio filter */
    if( !module_exists( "mpgatofixed32" ) )
        return VLC_EGENERIC;
    return Open( p_this );
}
