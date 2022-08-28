static int dash_flush(AVFormatContext *s, int final, int stream)
{
    DASHContext *c = s->priv_data;
    int i, ret = 0;
    int cur_flush_segment_index = 0;
    if (stream >= 0)
        cur_flush_segment_index = c->streams[stream].segment_index;
    for (i = 0; i < s->nb_streams; i++) {
        OutputStream *os = &c->streams[i];
        char filename[1024] = "", full_path[1024], temp_path[1024];
        int64_t start_pos;
        int range_length, index_length = 0;
        if (!os->packets_written)
            continue;
        
        
        
        if (stream >= 0 && i != stream) {
            if (s->streams[i]->codec->codec_type != AVMEDIA_TYPE_AUDIO)
                continue;
            
            
            if (c->has_video && os->segment_index > cur_flush_segment_index)
                continue;
        }
        if (!os->init_range_length) {
            av_write_frame(os->ctx, NULL);
            os->init_range_length = avio_tell(os->ctx->pb);
            if (!c->single_file) {
                ffurl_close(os->out);
                os->out = NULL;
            }
        }
        start_pos = avio_tell(os->ctx->pb);
        if (!c->single_file) {
            dash_fill_tmpl_params(filename, sizeof(filename), c->media_seg_name, i, os->segment_index, os->bit_rate, os->start_pts);
            snprintf(full_path, sizeof(full_path), "%s%s", c->dirname, filename);
            snprintf(temp_path, sizeof(temp_path), "%s.tmp", full_path);
            ret = ffurl_open(&os->out, temp_path, AVIO_FLAG_WRITE, &s->interrupt_callback, NULL);
            if (ret < 0)
                break;
            write_styp(os->ctx->pb);
        } else {
            snprintf(full_path, sizeof(full_path), "%s%s", c->dirname, os->initfile);
        }
        av_write_frame(os->ctx, NULL);
        avio_flush(os->ctx->pb);
        os->packets_written = 0;
        range_length = avio_tell(os->ctx->pb) - start_pos;
        if (c->single_file) {
            find_index_range(s, full_path, start_pos, &index_length);
        } else {
            ffurl_close(os->out);
            os->out = NULL;
            ret = ff_rename(temp_path, full_path, s);
            if (ret < 0)
                break;
        }
        add_segment(os, filename, os->start_pts, os->max_pts - os->start_pts, start_pos, range_length, index_length);
        av_log(s, AV_LOG_VERBOSE, "Representation %d media segment %d written to: %s\n", i, os->segment_index, full_path);
    }
    if (c->window_size || (final && c->remove_at_exit)) {
        for (i = 0; i < s->nb_streams; i++) {
            OutputStream *os = &c->streams[i];
            int j;
            int remove = os->nb_segments - c->window_size - c->extra_window_size;
            if (final && c->remove_at_exit)
                remove = os->nb_segments;
            if (remove > 0) {
                for (j = 0; j < remove; j++) {
                    char filename[1024];
                    snprintf(filename, sizeof(filename), "%s%s", c->dirname, os->segments[j]->file);
                    unlink(filename);
                    av_free(os->segments[j]);
                }
                os->nb_segments -= remove;
                memmove(os->segments, os->segments + remove, os->nb_segments * sizeof(*os->segments));
            }
        }
    }
    if (ret >= 0)
        ret = write_manifest(s, final);
    return ret;
}
