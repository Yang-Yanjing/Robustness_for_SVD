static int write_globalinfo(NUTContext *nut, AVIOContext *bc)
{
    AVFormatContext *s   = nut->avf;
    AVDictionaryEntry *t = NULL;
    AVIOContext *dyn_bc;
    uint8_t *dyn_buf = NULL;
    int count        = 0, dyn_size;
    int ret          = avio_open_dyn_buf(&dyn_bc);
    if (ret < 0)
        return ret;
    while ((t = av_dict_get(s->metadata, "", t, AV_DICT_IGNORE_SUFFIX)))
        count += add_info(dyn_bc, t->key, t->value);
    ff_put_v(bc, 0); 
    ff_put_v(bc, 0); 
    ff_put_v(bc, 0); 
    ff_put_v(bc, 0); 
    ff_put_v(bc, count);
    dyn_size = avio_close_dyn_buf(dyn_bc, &dyn_buf);
    avio_write(bc, dyn_buf, dyn_size);
    av_free(dyn_buf);
    return 0;
}
