char *
services_discovery_GetLocalizedName ( services_discovery_t * p_sd )
{
    return strdup( module_get_name( p_sd->p_module, true ) );
}
