static void put_ebml_float(AVIOContext *pb, unsigned int elementid, double val)
{
    put_ebml_id(pb, elementid);
    put_ebml_num(pb, 8, 0);
    avio_wb64(pb, av_double2int(val));
}
