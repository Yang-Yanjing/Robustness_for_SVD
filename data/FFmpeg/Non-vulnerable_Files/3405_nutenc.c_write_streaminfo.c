static int write_streaminfo(NUTContext *nut, AVIOContext *bc, int stream_id) {
    AVFormatContext *s= nut->avf;
    AVStream* st = s->streams[stream_id];
    AVDictionaryEntry *t = NULL;
    AVIOContext *dyn_bc;
    uint8_t *dyn_buf=NULL;
    int count=0, dyn_size, i;
    int ret = avio_open_dyn_buf(&dyn_bc);
    if (ret < 0)
        return ret;
    while ((t = av_dict_get(st->metadata, "", t, AV_DICT_IGNORE_SUFFIX)))
        count += add_info(dyn_bc, t->key, t->value);
    for (i=0; ff_nut_dispositions[i].flag; ++i) {
        if (st->disposition & ff_nut_dispositions[i].flag)
            count += add_info(dyn_bc, "Disposition", ff_nut_dispositions[i].str);
    }
    if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        uint8_t buf[256];
        if (st->r_frame_rate.num>0 && st->r_frame_rate.den>0)
            snprintf(buf, sizeof(buf), "%d/%d", st->r_frame_rate.num, st->r_frame_rate.den);
        else
            snprintf(buf, sizeof(buf), "%d/%d", st->codec->time_base.den, st->codec->time_base.num);
        count += add_info(dyn_bc, "r_frame_rate", buf);
    }
    dyn_size = avio_close_dyn_buf(dyn_bc, &dyn_buf);
    if (count) {
        ff_put_v(bc, stream_id + 1); 
        ff_put_v(bc, 0); 
        ff_put_v(bc, 0); 
        ff_put_v(bc, 0); 
        ff_put_v(bc, count);
        avio_write(bc, dyn_buf, dyn_size);
    }
    av_free(dyn_buf);
    return count;
}
