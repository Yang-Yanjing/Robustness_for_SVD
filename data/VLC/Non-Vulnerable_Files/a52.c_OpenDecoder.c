}
static int OpenDecoder( vlc_object_t *p_this )
{
    /* HACK: Don't use this codec if we don't have an a52 audio filter */
    if( !module_exists( "a52tofloat32" ) )
        return VLC_EGENERIC;
    return OpenCommon( p_this, false );
}
