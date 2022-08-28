static int parse_MP4ODescrTag(MP4DescrParseContext *d, int64_t off, int len)
{
    int id_flags;
    if (len < 2)
        return 0;
    id_flags = avio_rb16(&d->pb);
    if (!(id_flags & 0x0020)) { 
        update_offsets(&d->pb, &off, &len);
        return parse_mp4_descr_arr(d, off, len); 
    } else {
        return 0;
    }
}
