DEFINE_WRITER_CLASS(xml);
static av_cold int xml_init(WriterContext *wctx)
{
    XMLContext *xml = wctx->priv;
    if (xml->xsd_strict) {
        xml->fully_qualified = 1;
#define CHECK_COMPLIANCE(opt, opt_name)                                 \
        if (opt) {                                                      \
            av_log(wctx, AV_LOG_ERROR,                                  \
                   "XSD-compliant output selected but option '%s' was selected, XML output may be non-compliant.\n" \
                   "You need to disable such option with '-no%s'\n", opt_name, opt_name); \
            return AVERROR(EINVAL);                                     \
        }
        CHECK_COMPLIANCE(show_private_data, "private");
        CHECK_COMPLIANCE(show_value_unit,   "unit");
        CHECK_COMPLIANCE(use_value_prefix,  "prefix");
        if (do_show_frames && do_show_packets) {
            av_log(wctx, AV_LOG_ERROR,
                   "Interleaved frames and packets are not allowed in XSD. "
                   "Select only one between the -show_frames and the -show_packets options.\n");
            return AVERROR(EINVAL);
        }
    }
    return 0;
}
