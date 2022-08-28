static int mov_write_trailer(AVFormatContext *s)
{
    MOVMuxContext *mov = s->priv_data;
    AVIOContext *pb = s->pb;
    int res = 0;
    int i;
    int64_t moov_pos;
    



    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *trk = &mov->tracks[i];
        if (trk->enc->codec_id == AV_CODEC_ID_MOV_TEXT &&
            !trk->last_sample_is_subtitle_end) {
            mov_write_subtitle_end_packet(s, i, trk->track_duration);
            trk->last_sample_is_subtitle_end = 1;
        }
    }
    
    
    
    if (!mov->chapter_track && !(mov->flags & FF_MOV_FLAG_FRAGMENT)) {
        if (mov->mode & (MODE_MP4|MODE_MOV|MODE_IPOD) && s->nb_chapters) {
            mov->chapter_track = mov->nb_streams++;
            if ((res = mov_create_chapter_track(s, mov->chapter_track)) < 0)
                goto error;
        }
    }
    if (!(mov->flags & FF_MOV_FLAG_FRAGMENT)) {
        moov_pos = avio_tell(pb);
        
        if (mov->mdat_size + 8 <= UINT32_MAX) {
            avio_seek(pb, mov->mdat_pos, SEEK_SET);
            avio_wb32(pb, mov->mdat_size + 8);
        } else {
            
            avio_seek(pb, mov->mdat_pos - 8, SEEK_SET);
            

            avio_wb32(pb, 1);
            ffio_wfourcc(pb, "mdat");
            avio_wb64(pb, mov->mdat_size + 16);
        }
        avio_seek(pb, mov->reserved_moov_size > 0 ? mov->reserved_header_pos : moov_pos, SEEK_SET);
        if (mov->flags & FF_MOV_FLAG_FASTSTART) {
            av_log(s, AV_LOG_INFO, "Starting second pass: moving the moov atom to the beginning of the file\n");
            res = shift_data(s);
            if (res == 0) {
                avio_seek(pb, mov->reserved_header_pos, SEEK_SET);
                if ((res = mov_write_moov_tag(pb, mov, s)) < 0)
                    goto error;
            }
        } else if (mov->reserved_moov_size > 0) {
            int64_t size;
            if ((res = mov_write_moov_tag(pb, mov, s)) < 0)
                goto error;
            size = mov->reserved_moov_size - (avio_tell(pb) - mov->reserved_header_pos);
            if (size < 8){
                av_log(s, AV_LOG_ERROR, "reserved_moov_size is too small, needed %"PRId64" additional\n", 8-size);
                res = AVERROR(EINVAL);
                goto error;
            }
            avio_wb32(pb, size);
            ffio_wfourcc(pb, "free");
            ffio_fill(pb, 0, size - 8);
            avio_seek(pb, moov_pos, SEEK_SET);
        } else {
            if ((res = mov_write_moov_tag(pb, mov, s)) < 0)
                goto error;
        }
        res = 0;
    } else {
        mov_auto_flush_fragment(s);
        for (i = 0; i < mov->nb_streams; i++)
           mov->tracks[i].data_offset = 0;
        if (mov->flags & FF_MOV_FLAG_GLOBAL_SIDX) {
            av_log(s, AV_LOG_INFO, "Starting second pass: inserting sidx atoms\n");
            res = shift_data(s);
            if (res == 0) {
                int64_t end = avio_tell(pb);
                avio_seek(pb, mov->reserved_header_pos, SEEK_SET);
                mov_write_sidx_tags(pb, mov, -1, 0);
                avio_seek(pb, end, SEEK_SET);
                mov_write_mfra_tag(pb, mov);
            }
        } else {
            mov_write_mfra_tag(pb, mov);
        }
    }
error:
    mov_free(s);
    return res;
}
