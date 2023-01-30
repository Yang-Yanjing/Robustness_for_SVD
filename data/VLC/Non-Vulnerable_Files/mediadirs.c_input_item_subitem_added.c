 *****************************************************************************/
static void input_item_subitem_added( const vlc_event_t * p_event,
                                      void * user_data )
{
    services_discovery_t *p_sd = user_data;
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    /* retrieve new item */
    input_item_t *p_item = p_event->u.input_item_subitem_added.p_new_child;
    if( p_sys->i_type == Picture )
        formatSnapshotItem( p_item );
    services_discovery_AddItem( p_sd, p_item, NULL );
}
