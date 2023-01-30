};
static inline uint32_t CanonicaliseChannels( uint32_t i_physical_channels )
{
    for( unsigned i = 0; i < sizeof( valid_channels )/sizeof( valid_channels[0] ); i++ )
        if( (i_physical_channels & ~valid_channels[i]) == 0 )
            return valid_channels[i];
    assert( false );
    return 0;
}
