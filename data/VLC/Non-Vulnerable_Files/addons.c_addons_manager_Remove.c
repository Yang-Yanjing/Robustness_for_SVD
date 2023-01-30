}
int addons_manager_Remove( addons_manager_t *p_manager, const addon_uuid_t uuid )
{
    return addons_manager_Install( p_manager, uuid );
}
