 */
int Import_iTML( vlc_object_t *p_this )
{
    DEMUX_BY_EXTENSION_OR_FORCED_MSG( ".xml", "itml",
                                      "using iTunes Media Library reader" );
    return VLC_SUCCESS;
}
