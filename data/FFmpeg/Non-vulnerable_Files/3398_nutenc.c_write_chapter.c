static int write_chapter(NUTContext *nut, AVIOContext *bc, int id)
{
    AVIOContext *dyn_bc;
    uint8_t *dyn_buf     = NULL;
    AVDictionaryEntry *t = NULL;
    AVChapter *ch        = nut->avf->chapters[id];
    int ret, dyn_size, count = 0;
    ret = avio_open_dyn_buf(&dyn_bc);
    if (ret < 0)
        return ret;
    ff_put_v(bc, 0);                                        
    put_s(bc, id + 1);                                      
    put_tt(nut, nut->chapter[id].time_base, bc, ch->start); 
    ff_put_v(bc, ch->end - ch->start);                      
    while ((t = av_dict_get(ch->metadata, "", t, AV_DICT_IGNORE_SUFFIX)))
        count += add_info(dyn_bc, t->key, t->value);
    ff_put_v(bc, count);
    dyn_size = avio_close_dyn_buf(dyn_bc, &dyn_buf);
    avio_write(bc, dyn_buf, dyn_size);
    av_freep(&dyn_buf);
    return 0;
}
