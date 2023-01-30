}
static bool parse_tracks_dict( demux_t *p_demux, input_item_node_t *p_input_node,
                               track_elem_t *p_track, xml_reader_t *p_xml_reader,
                               const char *psz_element,
                               xml_elem_hnd_t *p_handlers )
{
    VLC_UNUSED(p_track); VLC_UNUSED(psz_element); VLC_UNUSED(p_handlers);
    xml_elem_hnd_t tracks_elements[] =
        { {"dict",    COMPLEX_CONTENT, {.cmplx = parse_track_dict} },
          {"key",     SIMPLE_CONTENT,  {NULL} },
          {NULL,      UNKNOWN_CONTENT, {NULL} }
        };
