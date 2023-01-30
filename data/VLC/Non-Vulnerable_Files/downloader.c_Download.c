}
static int Download( stream_t *s, sms_stream_t *sms )
{
    stream_sys_t *p_sys = s->p_sys;
    int index = es_cat_to_index( sms->type );
    if ( unlikely( index == -1 ) )
    {
        msg_Err( s, "invalid stream type" );
        return VLC_EGENERIC;
    }
    int64_t start_time = p_sys->download.lead[index];
    quality_level_t *qlevel = get_qlevel( sms, sms->download_qlvl );
    if( unlikely( !qlevel ) )
    {
        msg_Err( s, "Could not get quality level id %u", sms->download_qlvl );
        return VLC_EGENERIC;
    }
    chunk_t *chunk = chunk_Get( sms, start_time );
    if( !chunk )
    {
        msg_Warn( s, "Could not find a chunk for stream %s, "\
                "start time = %"PRIu64"", sms->name, start_time );
        return VLC_EGENERIC;
    }
    if( chunk->data != NULL )
    {
        /* Segment already downloaded */
        msg_Warn( s, "Segment already downloaded" );
        return VLC_SUCCESS;
    }
    chunk->type = sms->type;
    char *url = ConstructUrl( sms->url_template, p_sys->base_url,
                                  qlevel->Bitrate, chunk->start_time );
    if( !url )
    {
        msg_Err( s, "ConstructUrl returned NULL" );
        return VLC_EGENERIC;
    }
    /* sanity check - can we download this chunk on time? */
    uint64_t avg_bw = sms_queue_avg( p_sys->bws );
    if( (avg_bw > 0) && (qlevel->Bitrate > 0) )
    {
        /* duration in ms */
        unsigned chunk_duration = chunk->duration * 1000 / sms->timescale;
        uint64_t size = chunk_duration * qlevel->Bitrate / 1000; /* bits */
        unsigned estimated = size * 1000 / avg_bw;
        if( estimated > chunk_duration )
        {
            msg_Warn( s,"downloading of chunk %d would take %d ms, "\
                    "which is longer than its playback (%d ms)",
                        chunk->sequence, estimated, chunk_duration );
        }
    }
    mtime_t start = mdate();
    if( sms_Download( s, chunk, url ) != VLC_SUCCESS )
    {
        msg_Err( s, "downloaded chunk %u from stream %s at quality\
            %u failed", chunk->sequence, sms->name, qlevel->Bitrate );
        return VLC_EGENERIC;
    }
    mtime_t duration = mdate() - start;
    unsigned real_id = set_track_id( chunk, sms->id );
    if( real_id == 0)
    {
        msg_Err( s, "tfhd box not found or invalid chunk" );
        return VLC_EGENERIC;
    }
    //msg_Dbg( s, "chunk ID was %i and is now %i", real_id, sms->id );
    if( p_sys->b_live )
        get_new_chunks( s, chunk );
    vlc_mutex_lock( &p_sys->download.lock_wait );
    vlc_array_append( p_sys->download.chunks, chunk );
    vlc_cond_signal( &p_sys->download.wait );
    vlc_mutex_unlock( &p_sys->download.lock_wait );
    msg_Info( s, "downloaded chunk %d from stream %s at quality %u",
                chunk->sequence, sms->name, qlevel->Bitrate );
    uint64_t actual_lead = chunk->start_time + chunk->duration;
    int ind = es_cat_to_index( sms->type );
    p_sys->download.ck_index[ind] = chunk->sequence;
    p_sys->download.lead[ind] = __MIN( p_sys->download.lead[ind], actual_lead );
    if( sms->type == VIDEO_ES ||
            ( !SMS_GET_SELECTED_ST( VIDEO_ES ) && sms->type == AUDIO_ES ) )
    {
        p_sys->playback.toffset = __MIN( p_sys->playback.toffset,
                                            (uint64_t)chunk->start_time );
    }
    unsigned dur_ms = __MAX( 1, duration / 1000 );
    uint64_t bw = chunk->size * 8 * 1000 / dur_ms; /* bits / s */
    if( sms_queue_put( p_sys->bws, bw ) != VLC_SUCCESS )
        return VLC_EGENERIC;
    avg_bw = sms_queue_avg( p_sys->bws );
    if( sms->type != VIDEO_ES )
        return VLC_SUCCESS;
    /* Track could get disabled in mp4 demux if we trigger adaption too soon. */
    if( chunk->sequence <= 1 )
        return VLC_SUCCESS;
    unsigned new_qlevel_id = BandwidthAdaptation( s, sms, avg_bw );
    quality_level_t *new_qlevel = get_qlevel( sms, new_qlevel_id );
    if( unlikely( !new_qlevel ) )
    {
        msg_Err( s, "Could not get quality level id %u", new_qlevel_id );
        return VLC_EGENERIC;
    }
    if( new_qlevel->Bitrate != qlevel->Bitrate )
    {
        msg_Warn( s, "detected %s bandwidth (%u) stream",
                 (new_qlevel->Bitrate >= qlevel->Bitrate) ? "faster" : "lower",
                 new_qlevel->Bitrate );
        sms->download_qlvl = new_qlevel_id;
    }
    if( new_qlevel->MaxWidth != qlevel->MaxWidth ||
        new_qlevel->MaxHeight != qlevel->MaxHeight )
    {
        chunk_t *new_init_ck = build_init_chunk( s );
        if( !new_init_ck )
        {
            return VLC_EGENERIC;
        }
        new_init_ck->offset = p_sys->download.next_chunk_offset;
        p_sys->download.next_chunk_offset += new_init_ck->size;
        vlc_mutex_lock( &p_sys->download.lock_wait );
        vlc_array_append( p_sys->download.chunks, new_init_ck );
        vlc_array_append( p_sys->init_chunks, new_init_ck );
        vlc_mutex_unlock( &p_sys->download.lock_wait );
    }
    return VLC_SUCCESS;
}
