}
static int CountTracks( uint64_t const sent, uint64_t const total,
                        void const * const data )
{
    VLC_UNUSED( sent );
    services_discovery_t *p_sd = (services_discovery_t *)data;
    p_sd->p_sys->i_tracks_num = total;
    return 0;
}
