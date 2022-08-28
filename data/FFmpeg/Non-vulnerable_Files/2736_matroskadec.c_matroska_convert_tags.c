static void matroska_convert_tags(AVFormatContext *s)
{
    MatroskaDemuxContext *matroska = s->priv_data;
    MatroskaTags *tags = matroska->tags.elem;
    int i, j;
    for (i = 0; i < matroska->tags.nb_elem; i++) {
        if (tags[i].target.attachuid) {
            MatroskaAttachment *attachment = matroska->attachments.elem;
            for (j = 0; j < matroska->attachments.nb_elem; j++)
                if (attachment[j].uid == tags[i].target.attachuid &&
                    attachment[j].stream)
                    matroska_convert_tag(s, &tags[i].tag,
                                         &attachment[j].stream->metadata, NULL);
        } else if (tags[i].target.chapteruid) {
            MatroskaChapter *chapter = matroska->chapters.elem;
            for (j = 0; j < matroska->chapters.nb_elem; j++)
                if (chapter[j].uid == tags[i].target.chapteruid &&
                    chapter[j].chapter)
                    matroska_convert_tag(s, &tags[i].tag,
                                         &chapter[j].chapter->metadata, NULL);
        } else if (tags[i].target.trackuid) {
            MatroskaTrack *track = matroska->tracks.elem;
            for (j = 0; j < matroska->tracks.nb_elem; j++)
                if (track[j].uid == tags[i].target.trackuid && track[j].stream)
                    matroska_convert_tag(s, &tags[i].tag,
                                         &track[j].stream->metadata, NULL);
        } else {
            matroska_convert_tag(s, &tags[i].tag, &s->metadata,
                                 tags[i].target.type);
        }
    }
}
