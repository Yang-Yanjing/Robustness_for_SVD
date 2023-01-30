static void mxf_read_random_index_pack(AVFormatContext *s)
{
    MXFContext *mxf = s->priv_data;
    uint32_t length;
    int64_t file_size, max_rip_length, min_rip_length;
    KLVPacket klv;
    if (!s->pb->seekable)
        return;
    file_size = avio_size(s->pb);
    







    max_rip_length = ((file_size - mxf->run_in) / 105) * 12 + 28;
    max_rip_length = FFMIN(max_rip_length, INT_MAX); 
    
    min_rip_length = 16+1+24+4;
    
    avio_seek(s->pb, file_size - 4, SEEK_SET);
    length = avio_rb32(s->pb);
    if (length < min_rip_length || length > max_rip_length)
        goto end;
    avio_seek(s->pb, file_size - length, SEEK_SET);
    if (klv_read_packet(&klv, s->pb) < 0 ||
        !IS_KLV_KEY(klv.key, mxf_random_index_pack_key) ||
        klv.length != length - 20)
        goto end;
    avio_skip(s->pb, klv.length - 12);
    mxf->footer_partition = avio_rb64(s->pb);
    
    if (mxf->run_in + mxf->footer_partition >= file_size) {
        av_log(s, AV_LOG_WARNING, "bad FooterPartition in RIP - ignoring\n");
        mxf->footer_partition = 0;
    }
end:
    avio_seek(s->pb, mxf->run_in, SEEK_SET);
}
