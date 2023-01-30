char *
libvlc_media_discoverer_localized_name( libvlc_media_discoverer_t * p_mdis )
{
    return services_discovery_GetLocalizedName( p_mdis->p_sd );
}
