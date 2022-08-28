static int mkv_write_tracks(AVFormatContext *s)
{
    MatroskaMuxContext *mkv = s->priv_data;
    AVIOContext *pb = s->pb;
    ebml_master tracks;
    int i, ret, default_stream_exists = 0;
    ret = mkv_add_seekhead_entry(mkv->main_seekhead, MATROSKA_ID_TRACKS, avio_tell(pb));
    if (ret < 0)
        return ret;
    tracks = start_ebml_master(pb, MATROSKA_ID_TRACKS, 0);
    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        default_stream_exists |= st->disposition & AV_DISPOSITION_DEFAULT;
    }
    for (i = 0; i < s->nb_streams; i++) {
        ret = mkv_write_track(s, mkv, i, pb, default_stream_exists);
        if (ret < 0)
            return ret;
    }
    end_ebml_master(pb, tracks);
    return 0;
}
