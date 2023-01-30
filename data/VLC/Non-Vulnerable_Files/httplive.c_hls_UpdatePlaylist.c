   (which represents a downloaded, perhaps newer version of the same playlist) */
static int hls_UpdatePlaylist(stream_t *s, hls_stream_t *hls_new, hls_stream_t *hls_old, bool *stream_appended)
{
    int count = vlc_array_count(hls_new->segments);
    msg_Dbg(s, "updating hls stream (program-id=%d, bandwidth=%"PRIu64") has %d segments",
             hls_new->id, hls_new->bandwidth, count);
    vlc_mutex_lock(&hls_old->lock);
    hls_old->max_segment_length=-1;
    for (int n = 0; n < count; n++)
    {
        segment_t *p = segment_GetSegment(hls_new, n);
        if (p == NULL)
            continue;
        segment_t *segment = segment_Find(hls_old, p->sequence);
        if (segment)
        {
            vlc_mutex_lock(&segment->lock);
            assert(p->url);
            assert(segment->url);
            /* they should be the same */
            if ((p->sequence != segment->sequence) ||
                (p->duration != segment->duration) ||
                (strcmp(p->url, segment->url) != 0))
            {
                msg_Warn(s, "existing segment found with different content - resetting");
                msg_Warn(s, "- sequence: new=%d, old=%d", p->sequence, segment->sequence);
                msg_Warn(s, "- duration: new=%d, old=%d", p->duration, segment->duration);
                msg_Warn(s, "- file: new=%s", p->url);
                msg_Warn(s, "        old=%s", segment->url);
                /* Resetting content */
                segment->sequence = p->sequence;
                segment->duration = p->duration;
                free(segment->url);
                segment->url = strdup(p->url);
                if ( segment->url == NULL )
                {
                    msg_Err(s, "Failed updating segment %d - skipping it",  p->sequence);
                    segment_Free(p);
                    vlc_mutex_unlock(&segment->lock);
                    continue;
                }
                /* We must free the content, because if the key was not downloaded, content can't be decrypted */
                if ((p->psz_key_path || p->b_key_loaded) &&
                    segment->data)
                {
                    block_Release(segment->data);
                    segment->data = NULL;
                }
                free(segment->psz_key_path);
                segment->psz_key_path = p->psz_key_path ? strdup(p->psz_key_path) : NULL;
            }
            segment_Free(p);
            vlc_mutex_unlock(&segment->lock);
        }
        else
        {
            int last = vlc_array_count(hls_old->segments) - 1;
            segment_t *l = segment_GetSegment(hls_old, last);
            if (l == NULL) {
                vlc_mutex_unlock(&hls_old->lock);
                segment_Free(p);
                continue;
            }
            if ((l->sequence + 1) != p->sequence)
            {
                msg_Err(s, "gap in sequence numbers found: new=%d expected %d",
                        p->sequence, l->sequence+1);
            }
            vlc_array_append(hls_old->segments, p);
            msg_Dbg(s, "- segment %d appended", p->sequence);
            hls_old->max_segment_length = __MAX(hls_old->max_segment_length, p->duration);
            msg_Dbg(s, "  - segments new max duration %d", hls_old->max_segment_length);
            // Signal download thread otherwise the segment will not get downloaded
            *stream_appended = true;
        }
    }
    /* update meta information */
    hls_old->sequence = hls_new->sequence;
    hls_old->duration = (hls_new->duration == -1) ? hls_old->duration : hls_new->duration;
    hls_old->b_cache = hls_new->b_cache;
    vlc_mutex_unlock(&hls_old->lock);
    vlc_array_clear(hls_new->segments);
    return VLC_SUCCESS;
}
