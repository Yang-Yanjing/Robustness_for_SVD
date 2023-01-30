 ***********************************************************************/
void vlc_sd_Destroy( services_discovery_t *p_sd )
{
    config_ChainDestroy( p_sd->p_cfg );
    free( p_sd->psz_name );
    vlc_object_release( p_sd );
}
