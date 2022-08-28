static int write_headers(AVFormatContext *avctx, AVIOContext *bc)
{
    NUTContext *nut = avctx->priv_data;
    AVIOContext *dyn_bc;
    int i, ret;
    ff_metadata_conv_ctx(avctx, ff_nut_metadata_conv, NULL);
    ret = avio_open_dyn_buf(&dyn_bc);
    if (ret < 0)
        return ret;
    write_mainheader(nut, dyn_bc);
    put_packet(nut, bc, dyn_bc, 1, MAIN_STARTCODE);
    for (i = 0; i < nut->avf->nb_streams; i++) {
        ret = avio_open_dyn_buf(&dyn_bc);
        if (ret < 0)
            return ret;
        ret = write_streamheader(avctx, dyn_bc, nut->avf->streams[i], i);
        if (ret < 0)
            return ret;
        put_packet(nut, bc, dyn_bc, 1, STREAM_STARTCODE);
    }
    ret = avio_open_dyn_buf(&dyn_bc);
    if (ret < 0)
        return ret;
    write_globalinfo(nut, dyn_bc);
    put_packet(nut, bc, dyn_bc, 1, INFO_STARTCODE);
    for (i = 0; i < nut->avf->nb_streams; i++) {
        ret = avio_open_dyn_buf(&dyn_bc);
        if (ret < 0)
            return ret;
        ret = write_streaminfo(nut, dyn_bc, i);
        if (ret < 0)
            return ret;
        if (ret > 0)
            put_packet(nut, bc, dyn_bc, 1, INFO_STARTCODE);
        else
            ffio_free_dyn_buf(&dyn_bc);
    }
    for (i = 0; i < nut->avf->nb_chapters; i++) {
        ret = avio_open_dyn_buf(&dyn_bc);
        if (ret < 0)
            return ret;
        ret = write_chapter(nut, dyn_bc, i);
        if (ret < 0) {
            ffio_free_dyn_buf(&dyn_bc);
            return ret;
        }
        put_packet(nut, bc, dyn_bc, 1, INFO_STARTCODE);
    }
    nut->last_syncpoint_pos = INT_MIN;
    nut->header_count++;
    return 0;
}
