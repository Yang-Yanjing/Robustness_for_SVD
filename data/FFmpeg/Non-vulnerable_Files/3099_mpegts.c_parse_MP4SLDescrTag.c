static int parse_MP4SLDescrTag(MP4DescrParseContext *d, int64_t off, int len)
{
    Mp4Descr *descr = d->active_descr;
    int predefined;
    if (!descr)
        return AVERROR_INVALIDDATA;
    predefined = avio_r8(&d->pb);
    if (!predefined) {
        int lengths;
        int flags = avio_r8(&d->pb);
        descr->sl.use_au_start    = !!(flags & 0x80);
        descr->sl.use_au_end      = !!(flags & 0x40);
        descr->sl.use_rand_acc_pt = !!(flags & 0x20);
        descr->sl.use_padding     = !!(flags & 0x08);
        descr->sl.use_timestamps  = !!(flags & 0x04);
        descr->sl.use_idle        = !!(flags & 0x02);
        descr->sl.timestamp_res   = avio_rb32(&d->pb);
        avio_rb32(&d->pb);
        descr->sl.timestamp_len      = avio_r8(&d->pb);
        if (descr->sl.timestamp_len > 64) {
            avpriv_request_sample(NULL, "timestamp_len > 64");
            descr->sl.timestamp_len = 64;
            return AVERROR_PATCHWELCOME;
        }
        descr->sl.ocr_len            = avio_r8(&d->pb);
        descr->sl.au_len             = avio_r8(&d->pb);
        descr->sl.inst_bitrate_len   = avio_r8(&d->pb);
        lengths                      = avio_rb16(&d->pb);
        descr->sl.degr_prior_len     = lengths >> 12;
        descr->sl.au_seq_num_len     = (lengths >> 7) & 0x1f;
        descr->sl.packet_seq_num_len = (lengths >> 2) & 0x1f;
    } else if (!d->predefined_SLConfigDescriptor_seen){
        avpriv_report_missing_feature(d->s, "Predefined SLConfigDescriptor");
        d->predefined_SLConfigDescriptor_seen = 1;
    }
    return 0;
}
