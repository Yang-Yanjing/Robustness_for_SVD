 ***********************************************************************/
static void services_discovery_Destructor ( vlc_object_t *p_obj )
{
    services_discovery_t * p_sd = (services_discovery_t *)p_obj;
    assert(!p_sd->p_module); /* Forgot to call Stop */
    vlc_event_manager_fini( &p_sd->event_manager );
}
