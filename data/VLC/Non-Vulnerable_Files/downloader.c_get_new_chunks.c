#endif
static int get_new_chunks( stream_t *s, chunk_t *ck )
{
    stream_sys_t *p_sys = s->p_sys;
    uint8_t *slice = ck->data;
    if( !slice )
        return VLC_EGENERIC;
    uint8_t version, fragment_count;
    uint32_t size, type, flags;
    sms_stream_t *sms;
    UUID_t uuid;
    TfrfBoxDataFields_t *tfrf_df;
    sms = SMS_GET_SELECTED_ST( ck->type );
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
    for(;;)
    {
        SMS_GET4BYTES( size );
        assert( size > 1 );
        SMS_GETFOURCC( type );
        if( type == ATOM_mdat )
        {
            msg_Err( s, "No uuid box found :-(" );
            return VLC_EGENERIC;
        }
        else if( type == ATOM_uuid )
        {
            GetUUID( &uuid, slice);
            if( !CmpUUID( &uuid, &TfrfBoxUUID ) )
                break;
        }
        slice += size - 8;
    }
    slice += 16;
    SMS_GET1BYTE( version );
    SMS_GET3BYTES( flags );
    SMS_GET1BYTE( fragment_count );
    tfrf_df = calloc( fragment_count, sizeof( TfrfBoxDataFields_t ) );
    if( unlikely( tfrf_df == NULL ) )
        return VLC_EGENERIC;
    for( uint8_t i = 0; i < fragment_count; i++ )
    {
        SMS_GET4or8BYTES( tfrf_df[i].i_fragment_abs_time );
        SMS_GET4or8BYTES( tfrf_df[i].i_fragment_duration );
    }
    msg_Dbg( s, "read box: \"tfrf\" version %d, flags 0x%x, "\
            "fragment count %"PRIu8, version, flags, fragment_count );
    for( uint8_t i = 0; i < fragment_count; i++ )
    {
        int64_t dur = tfrf_df[i].i_fragment_duration;
        int64_t stime = tfrf_df[i].i_fragment_abs_time;
        msg_Dbg( s, "\"tfrf\" fragment duration %"PRIu64", "\
                    "fragment abs time %"PRIu64, dur, stime);
        if( !chunk_Get( sms, stime + dur ) )
            chunk_New( sms, dur, stime );
    }
    free( tfrf_df );
    return VLC_SUCCESS;
}
