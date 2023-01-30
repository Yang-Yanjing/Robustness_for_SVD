 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    /* HACK: Don't use this codec if we don't have an dts audio filter */
    if( !module_exists( "dtstofloat32" ) )
        return VLC_EGENERIC;
    return OpenCommon( p_this, false );
}
