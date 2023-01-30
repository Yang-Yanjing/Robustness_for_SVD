}
static int RenderHtml(filter_t *p_filter, subpicture_region_t *p_region_out,
                       subpicture_region_t *p_region_in,
                       const vlc_fourcc_t *p_chroma_list)
{
    int          rv = VLC_SUCCESS;
    stream_t     *p_sub = NULL;
    xml_t        *p_xml = NULL;
    xml_reader_t *p_xml_reader = NULL;
    VLC_UNUSED(p_chroma_list);
    if (!p_region_in || !p_region_in->psz_html)
        return VLC_EGENERIC;
    /* Reset the default fontsize in case screen metrics have changed */
    p_filter->p_sys->i_font_size = GetFontSize(p_filter);
    p_sub = stream_MemoryNew(VLC_OBJECT(p_filter),
                              (uint8_t *) p_region_in->psz_html,
                              strlen(p_region_in->psz_html),
                              true);
    if (p_sub) {
        p_xml = xml_Create(p_filter);
        if (p_xml) {
            p_xml_reader = xml_ReaderCreate(p_xml, p_sub);
            if (p_xml_reader) {
                /* Look for Root Node */
                const char *name;
                if (xml_ReaderNextNode(p_xml_reader, &name)
                        == XML_READER_STARTELEM) {
                    if (!strcasecmp("karaoke", name)) {
                        /* We're going to have to render the text a number
                         * of times to show the progress marker on the text.
                         */
                        var_SetBool(p_filter, "text-rerender", true);
                    } else if (strcasecmp("text", name)) {
                        /* Only text and karaoke tags are supported */
                        msg_Dbg(p_filter, "Unsupported top-level tag "
                                           "<%s> ignored.", name);
                        rv = VLC_EGENERIC;
                    }
                } else {
                    msg_Err(p_filter, "Malformed HTML subtitle");
                    rv = VLC_EGENERIC;
                }
                if (rv != VLC_SUCCESS) {
                    xml_ReaderDelete(p_xml_reader);
                    p_xml_reader = NULL;
                }
            }
            if (p_xml_reader) {
                int         i_len;
                CFMutableAttributedStringRef p_attrString = CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
                rv = ProcessNodes(p_filter, p_xml_reader,
                              p_region_in->p_style, p_attrString);
                i_len = CFAttributedStringGetLength(p_attrString);
                p_region_out->i_x = p_region_in->i_x;
                p_region_out->i_y = p_region_in->i_y;
                if ((rv == VLC_SUCCESS) && (i_len > 0))
                    RenderYUVA(p_filter, p_region_out, p_attrString);
                CFRelease(p_attrString);
                xml_ReaderDelete(p_xml_reader);
            }
            xml_Delete(p_xml);
        }
        stream_Delete(p_sub);
    }
    return rv;
}
