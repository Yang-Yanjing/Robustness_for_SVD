static int mov_write_fiel_tag(AVIOContext *pb, MOVTrack *track)
{
    unsigned mov_field_order = 0;
    if (track->enc->field_order < FF_ARRAY_ELEMS(fiel_data))
        mov_field_order = fiel_data[track->enc->field_order];
    else
        return 0;
    avio_wb32(pb, 10);
    ffio_wfourcc(pb, "fiel");
    avio_wb16(pb, mov_field_order);
    return 10;
}
