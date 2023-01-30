static void show_packet(WriterContext *w, InputFile *ifile, AVPacket *pkt, int packet_idx)
{
    char val_str[128];
    AVStream *st = ifile->streams[pkt->stream_index].st;
    AVBPrint pbuf;
    const char *s;
    av_bprint_init(&pbuf, 1, AV_BPRINT_SIZE_UNLIMITED);
    writer_print_section_header(w, SECTION_ID_PACKET);
    s = av_get_media_type_string(st->codecpar->codec_type);
    if (s) print_str    ("codec_type", s);
    else   print_str_opt("codec_type", "unknown");
    print_int("stream_index",     pkt->stream_index);
    print_ts  ("pts",             pkt->pts);
    print_time("pts_time",        pkt->pts, &st->time_base);
    print_ts  ("dts",             pkt->dts);
    print_time("dts_time",        pkt->dts, &st->time_base);
    print_duration_ts("duration",        pkt->duration);
    print_duration_time("duration_time", pkt->duration, &st->time_base);
    print_duration_ts("convergence_duration", pkt->convergence_duration);
    print_duration_time("convergence_duration_time", pkt->convergence_duration, &st->time_base);
    print_val("size",             pkt->size, unit_byte_str);
    if (pkt->pos != -1) print_fmt    ("pos", "%"PRId64, pkt->pos);
    else                print_str_opt("pos", "N/A");
    print_fmt("flags", "%c%c",      pkt->flags & AV_PKT_FLAG_KEY ? 'K' : '_',
              pkt->flags & AV_PKT_FLAG_DISCARD ? 'D' : '_');
    if (pkt->side_data_elems) {
        int size;
        const uint8_t *side_metadata;
        side_metadata = av_packet_get_side_data(pkt, AV_PKT_DATA_STRINGS_METADATA, &size);
        if (side_metadata && size && do_show_packet_tags) {
            AVDictionary *dict = NULL;
            if (av_packet_unpack_dictionary(side_metadata, size, &dict) >= 0)
                show_tags(w, dict, SECTION_ID_PACKET_TAGS);
            av_dict_free(&dict);
        }
        print_pkt_side_data(w, st->codecpar, pkt->side_data, pkt->side_data_elems,
                            SECTION_ID_PACKET_SIDE_DATA_LIST,
                            SECTION_ID_PACKET_SIDE_DATA);
    }
    if (do_show_data)
        writer_print_data(w, "data", pkt->data, pkt->size);
    writer_print_data_hash(w, "data_hash", pkt->data, pkt->size);
    writer_print_section_footer(w);
    av_bprint_finalize(&pbuf, NULL);
    fflush(stdout);
}
