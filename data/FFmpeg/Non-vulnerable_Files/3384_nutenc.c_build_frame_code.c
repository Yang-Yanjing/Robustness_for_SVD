static void build_frame_code(AVFormatContext *s)
{
    NUTContext *nut = s->priv_data;
    int key_frame, index, pred, stream_id;
    int start = 1;
    int end   = 254;
    int keyframe_0_esc = s->nb_streams > 2;
    int pred_table[10];
    FrameCode *ft;
    ft            = &nut->frame_code[start];
    ft->flags     = FLAG_CODED;
    ft->size_mul  = 1;
    ft->pts_delta = 1;
    start++;
    if (keyframe_0_esc) {
        
        FrameCode *ft = &nut->frame_code[start];
        ft->flags    = FLAG_STREAM_ID | FLAG_SIZE_MSB | FLAG_CODED_PTS;
        ft->size_mul = 1;
        start++;
    }
    for (stream_id = 0; stream_id < s->nb_streams; stream_id++) {
        int start2 = start + (end - start) * stream_id       / s->nb_streams;
        int end2   = start + (end - start) * (stream_id + 1) / s->nb_streams;
        AVCodecContext *codec = s->streams[stream_id]->codec;
        int is_audio          = codec->codec_type == AVMEDIA_TYPE_AUDIO;
        int intra_only        =  is_audio;
        int pred_count;
        int frame_size = 0;
        if (codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            frame_size = av_get_audio_frame_duration(codec, 0);
            if (codec->codec_id == AV_CODEC_ID_VORBIS && !frame_size)
                frame_size = 64;
        } else {
            AVRational f = av_div_q(codec->time_base, *nut->stream[stream_id].time_base);
            if (f.den == 1 && f.num>0)
                frame_size = f.num;
        }
        if (!frame_size)
            frame_size = 1;
        for (key_frame = 0; key_frame < 2; key_frame++) {
            if (!intra_only || !keyframe_0_esc || key_frame != 0) {
                FrameCode *ft = &nut->frame_code[start2];
                ft->flags     = FLAG_KEY * key_frame;
                ft->flags    |= FLAG_SIZE_MSB | FLAG_CODED_PTS;
                ft->stream_id = stream_id;
                ft->size_mul  = 1;
                if (is_audio)
                    ft->header_idx = find_header_idx(s, codec, -1, key_frame);
                start2++;
            }
        }
        key_frame = intra_only;
#if 1
        if (is_audio) {
            int frame_bytes = codec->frame_size * (int64_t)codec->bit_rate /
                              (8 * codec->sample_rate);
            int pts;
            for (pts = 0; pts < 2; pts++) {
                for (pred = 0; pred < 2; pred++) {
                    FrameCode *ft  = &nut->frame_code[start2];
                    ft->flags      = FLAG_KEY * key_frame;
                    ft->stream_id  = stream_id;
                    ft->size_mul   = frame_bytes + 2;
                    ft->size_lsb   = frame_bytes + pred;
                    ft->pts_delta  = pts * frame_size;
                    ft->header_idx = find_header_idx(s, codec, frame_bytes + pred, key_frame);
                    start2++;
                }
            }
        } else {
            FrameCode *ft = &nut->frame_code[start2];
            ft->flags     = FLAG_KEY | FLAG_SIZE_MSB;
            ft->stream_id = stream_id;
            ft->size_mul  = 1;
            ft->pts_delta = frame_size;
            start2++;
        }
#endif
        if (codec->has_b_frames) {
            pred_count    = 5;
            pred_table[0] = -2;
            pred_table[1] = -1;
            pred_table[2] = 1;
            pred_table[3] = 3;
            pred_table[4] = 4;
        } else if (codec->codec_id == AV_CODEC_ID_VORBIS) {
            pred_count    = 3;
            pred_table[0] = 2;
            pred_table[1] = 9;
            pred_table[2] = 16;
        } else {
            pred_count    = 1;
            pred_table[0] = 1;
        }
        for (pred = 0; pred < pred_count; pred++) {
            int start3 = start2 + (end2 - start2) * pred / pred_count;
            int end3   = start2 + (end2 - start2) * (pred + 1) / pred_count;
            pred_table[pred] *= frame_size;
            for (index = start3; index < end3; index++) {
                FrameCode *ft = &nut->frame_code[index];
                ft->flags     = FLAG_KEY * key_frame;
                ft->flags    |= FLAG_SIZE_MSB;
                ft->stream_id = stream_id;

                ft->size_mul  = end3 - start3;
                ft->size_lsb  = index - start3;
                ft->pts_delta = pred_table[pred];
                if (is_audio)
                    ft->header_idx = find_header_idx(s, codec, -1, key_frame);
            }
        }
    }
    memmove(&nut->frame_code['N' + 1], &nut->frame_code['N'], sizeof(FrameCode) * (255 - 'N'));
    nut->frame_code[0].flags       =
        nut->frame_code[255].flags =
        nut->frame_code['N'].flags = FLAG_INVALID;
}
