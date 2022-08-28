static int parse_MP4IODescrTag(MP4DescrParseContext *d, int64_t off, int len)
{
    avio_rb16(&d->pb); 
    avio_r8(&d->pb);
    avio_r8(&d->pb);
    avio_r8(&d->pb);
    avio_r8(&d->pb);
    avio_r8(&d->pb);
    update_offsets(&d->pb, &off, &len);
    return parse_mp4_descr_arr(d, off, len);
}
