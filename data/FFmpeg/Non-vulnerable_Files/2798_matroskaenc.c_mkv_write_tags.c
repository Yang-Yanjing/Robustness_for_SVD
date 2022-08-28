static int mkv_write_tags(AVFormatContext *s)
{
    MatroskaMuxContext *mkv = s->priv_data;
    ebml_master tags = {0};
    int i, ret;
    ff_metadata_conv_ctx(s, ff_mkv_metadata_conv, NULL);
    if (mkv_check_tag(s->metadata)) {
        ret = mkv_write_tag(s, s->metadata, 0, 0, &tags);
        if (ret < 0) return ret;
    }
    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        if (!mkv_check_tag(st->metadata))
            continue;
        ret = mkv_write_tag(s, st->metadata, MATROSKA_ID_TAGTARGETS_TRACKUID, i + 1, &tags);
        if (ret < 0) return ret;
    }
    if (!mkv->is_live) {
        for (i = 0; i < s->nb_streams; i++) {
            ebml_master tag_target;
            ebml_master tag;
            mkv_write_tag_targets(s, MATROSKA_ID_TAGTARGETS_TRACKUID, i + 1, &tags, &tag_target);
            tag = start_ebml_master(s->pb, MATROSKA_ID_SIMPLETAG, 0);
            put_ebml_string(s->pb, MATROSKA_ID_TAGNAME, "DURATION");
            mkv->stream_duration_offsets[i] = avio_tell(s->pb);
            
            
            put_ebml_void(s->pb, 23);
            end_ebml_master(s->pb, tag);
            end_ebml_master(s->pb, tag_target);
        }
    }
    for (i = 0; i < s->nb_chapters; i++) {
        AVChapter *ch = s->chapters[i];
        if (!mkv_check_tag(ch->metadata))
            continue;
        ret = mkv_write_tag(s, ch->metadata, MATROSKA_ID_TAGTARGETS_CHAPTERUID, ch->id + mkv->chapter_id_offset, &tags);
        if (ret < 0) return ret;
    }
    if (tags.pos)
        end_ebml_master(s->pb, tags);
    return 0;
}
