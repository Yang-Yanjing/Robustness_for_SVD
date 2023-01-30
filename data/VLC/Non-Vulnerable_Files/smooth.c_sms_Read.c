}
static int sms_Read( stream_t *s, uint8_t *p_read, int i_read )
{
    stream_sys_t *p_sys = s->p_sys;
    int copied = 0;
    chunk_t *chunk = NULL;
    do
    {
        chunk = get_chunk( s, true );
        if( !chunk )
            return copied;
        if( chunk->read_pos >= (int)chunk->size )
        {
            if( chunk->type == VIDEO_ES ||
                ( !SMS_GET_SELECTED_ST( VIDEO_ES ) && chunk->type == AUDIO_ES ) )
            {
                vlc_mutex_lock( &p_sys->download.lock_wait );
                p_sys->playback.toffset += chunk->duration;
                vlc_mutex_unlock( &p_sys->download.lock_wait );
                vlc_cond_signal( &p_sys->download.wait);
            }
            if( !p_sys->b_cache || p_sys->b_live )
            {
                FREENULL( chunk->data );
                chunk->read_pos = 0;
            }
            chunk->read_pos = 0;
            p_sys->playback.index += 1;
            msg_Dbg( s, "Incrementing playback index" );
            continue;
        }
        if( chunk->read_pos == 0 )
        {
            const char *verb = p_read == NULL ? "skipping" : "reading";
            msg_Dbg( s, "%s chunk %u (%u bytes), type %i",
                        verb, chunk->sequence, i_read, chunk->type );
            /* check integrity */
            uint32_t type;
            uint8_t *slice = chunk->data;
            SMS_GET4BYTES( type );
            SMS_GETFOURCC( type );
            assert( type == ATOM_moof || type == ATOM_uuid );
        }
        int len = -1;
        uint8_t *src = chunk->data + chunk->read_pos;
        if( i_read <= chunk->size - chunk->read_pos )
            len = i_read;
        else
            len = chunk->size - chunk->read_pos;
        if( len > 0 )
        {
            if( p_read ) /* otherwise caller skips data */
                memcpy( p_read + copied, src, len );
            chunk->read_pos += len;
            copied += len;
            i_read -= len;
        }
    } while ( i_read > 0 );
    return copied;
}
