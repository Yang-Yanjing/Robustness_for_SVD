}
int addons_manager_Install( addons_manager_t *p_manager, const addon_uuid_t uuid )
{
    addon_entry_t *p_install_entry = getHeldEntryByUUID( p_manager, uuid );
    if ( ! p_install_entry ) return VLC_EGENERIC;
    int i_ret = InstallEntry( p_manager, p_install_entry );
    addon_entry_Release( p_install_entry );
    return i_ret;
}
