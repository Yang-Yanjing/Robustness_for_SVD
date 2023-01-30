}
static unsigned set_track_id( chunk_t *chunk, const unsigned tid )
{
    uint32_t size, type;
    if( !chunk->data )
        return 0;
    uint8_t *slice = chunk->data;
    if( !slice )
        return 0;
    SMS_GET4BYTES( size );
    SMS_GETFOURCC( type );
    assert( type == ATOM_moof );
    SMS_GET4BYTES( size );
    SMS_GETFOURCC( type );
    assert( type == ATOM_mfhd );
    slice += size - 8;
    SMS_GET4BYTES( size );
    SMS_GETFOURCC( type );
    assert( type == ATOM_traf );
    SMS_GET4BYTES( size );
    SMS_GETFOURCC( type );
    if( type != ATOM_tfhd )
        return 0;
    unsigned ret = bswap32( ((uint32_t *)slice)[1] );
    ((uint32_t *)slice)[1] = bswap32( tid );
    return ret;
}
