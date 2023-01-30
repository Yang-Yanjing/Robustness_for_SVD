static void put_ebml_void(AVIOContext *pb, uint64_t size)
{
    int64_t currentpos = avio_tell(pb);
    av_assert0(size >= 2);
    put_ebml_id(pb, EBML_ID_VOID);
    
    
    
    if (size < 10)
        put_ebml_num(pb, size - 1, 0);
    else
        put_ebml_num(pb, size - 9, 8);
    ffio_fill(pb, 0, currentpos + size - avio_tell(pb));
}
