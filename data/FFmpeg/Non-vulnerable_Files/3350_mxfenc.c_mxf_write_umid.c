static void mxf_write_umid(AVFormatContext *s, int type)
{
    MXFContext *mxf = s->priv_data;
    avio_write(s->pb, umid_ul, 13);
    avio_wb24(s->pb, mxf->instance_number);
    avio_write(s->pb, mxf->umid, 15);
    avio_w8(s->pb, type);
}
