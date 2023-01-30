static int mkv_write_attachments(AVFormatContext *s)
{
    MatroskaMuxContext *mkv = s->priv_data;
    AVIOContext *pb = s->pb;
    ebml_master attachments;
    AVLFG c;
    int i, ret;
    if (!mkv->have_attachments)
        return 0;
    av_lfg_init(&c, av_get_random_seed());
    ret = mkv_add_seekhead_entry(mkv->main_seekhead, MATROSKA_ID_ATTACHMENTS, avio_tell(pb));
    if (ret < 0) return ret;
    attachments = start_ebml_master(pb, MATROSKA_ID_ATTACHMENTS, 0);
    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        ebml_master attached_file;
        AVDictionaryEntry *t;
        const char *mimetype = NULL;
        uint64_t fileuid;
        if (st->codec->codec_type != AVMEDIA_TYPE_ATTACHMENT)
            continue;
        attached_file = start_ebml_master(pb, MATROSKA_ID_ATTACHEDFILE, 0);
        if (t = av_dict_get(st->metadata, "title", NULL, 0))
            put_ebml_string(pb, MATROSKA_ID_FILEDESC, t->value);
        if (!(t = av_dict_get(st->metadata, "filename", NULL, 0))) {
            av_log(s, AV_LOG_ERROR, "Attachment stream %d has no filename tag.\n", i);
            return AVERROR(EINVAL);
        }
        put_ebml_string(pb, MATROSKA_ID_FILENAME, t->value);
        if (t = av_dict_get(st->metadata, "mimetype", NULL, 0))
            mimetype = t->value;
        else if (st->codec->codec_id != AV_CODEC_ID_NONE ) {
            int i;
            for (i = 0; ff_mkv_mime_tags[i].id != AV_CODEC_ID_NONE; i++)
                if (ff_mkv_mime_tags[i].id == st->codec->codec_id) {
                    mimetype = ff_mkv_mime_tags[i].str;
                    break;
                }
            for (i = 0; ff_mkv_image_mime_tags[i].id != AV_CODEC_ID_NONE; i++)
                if (ff_mkv_image_mime_tags[i].id == st->codec->codec_id) {
                    mimetype = ff_mkv_image_mime_tags[i].str;
                    break;
                }
        }
        if (!mimetype) {
            av_log(s, AV_LOG_ERROR, "Attachment stream %d has no mimetype tag and "
                                    "it cannot be deduced from the codec id.\n", i);
            return AVERROR(EINVAL);
        }
        if (s->flags & AVFMT_FLAG_BITEXACT) {
            struct AVSHA *sha = av_sha_alloc();
            uint8_t digest[20];
            if (!sha)
                return AVERROR(ENOMEM);
            av_sha_init(sha, 160);
            av_sha_update(sha, st->codec->extradata, st->codec->extradata_size);
            av_sha_final(sha, digest);
            av_free(sha);
            fileuid = AV_RL64(digest);
        } else {
            fileuid = av_lfg_get(&c);
        }
        av_log(s, AV_LOG_VERBOSE, "Using %.16"PRIx64" for attachment %d\n",
               fileuid, i);
        put_ebml_string(pb, MATROSKA_ID_FILEMIMETYPE, mimetype);
        put_ebml_binary(pb, MATROSKA_ID_FILEDATA, st->codec->extradata, st->codec->extradata_size);
        put_ebml_uint(pb, MATROSKA_ID_FILEUID, fileuid);
        end_ebml_master(pb, attached_file);
    }
    end_ebml_master(pb, attachments);
    return 0;
}
