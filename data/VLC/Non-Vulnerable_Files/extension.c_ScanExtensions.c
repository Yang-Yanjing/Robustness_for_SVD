 **/
static int ScanExtensions( extensions_manager_t *p_mgr )
{
    int i_ret =
        vlclua_scripts_batch_execute( VLC_OBJECT( p_mgr ),
                                      "extensions",
                                      &ScanLuaCallback,
                                      NULL );
    if( !i_ret )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
