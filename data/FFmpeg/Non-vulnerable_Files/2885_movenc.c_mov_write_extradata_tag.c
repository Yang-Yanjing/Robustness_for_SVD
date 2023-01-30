static int mov_write_extradata_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_write(pb, track->enc->extradata, track->enc->extradata_size);
    return track->enc->extradata_size;
}
