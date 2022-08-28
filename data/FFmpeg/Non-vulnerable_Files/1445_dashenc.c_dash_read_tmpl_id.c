static DASHTmplId dash_read_tmpl_id(const char *identifier, char *format_tag,
                                    size_t format_tag_size, const char **ptr) {
    const char *next_ptr;
    DASHTmplId id_type = DASH_TMPL_ID_UNDEFINED;
    if (av_strstart(identifier, "$$", &next_ptr)) {
        id_type = DASH_TMPL_ID_ESCAPE;
        *ptr = next_ptr;
    } else if (av_strstart(identifier, "$RepresentationID$", &next_ptr)) {
        id_type = DASH_TMPL_ID_REP_ID;
        
        
        av_strlcpy(format_tag, "%d", format_tag_size);
        *ptr = next_ptr;
    } else { 
        if (av_strstart(identifier, "$Number", &next_ptr))
            id_type = DASH_TMPL_ID_NUMBER;
        else if (av_strstart(identifier, "$Bandwidth", &next_ptr))
            id_type = DASH_TMPL_ID_BANDWIDTH;
        else if (av_strstart(identifier, "$Time", &next_ptr))
            id_type = DASH_TMPL_ID_TIME;
        else
            id_type = DASH_TMPL_ID_UNDEFINED;
        
        
        if (id_type != DASH_TMPL_ID_UNDEFINED) {
            const char *number_format = (id_type == DASH_TMPL_ID_TIME) ? PRId64 : "d";
            if (next_ptr[0] == '$') { 
                snprintf(format_tag, format_tag_size, "%%%s", number_format);
                *ptr = &next_ptr[1];
            } else {
                const char *width_ptr;
                
                if (av_strstart(next_ptr, "%0", &width_ptr) &&
                    av_isdigit(width_ptr[0]) &&
                    av_strstart(&width_ptr[1], "d$", &next_ptr)) {
                    
                    snprintf(format_tag, format_tag_size, "%s%c%s", "%0", width_ptr[0], number_format);
                    *ptr = next_ptr;
                } else {
                    av_log(NULL, AV_LOG_WARNING, "Failed to parse format-tag beginning with %s. Expected either a "
                                                 "closing '$' character or a format-string like '%%0[width]d', "
                                                 "where width must be a single digit\n", next_ptr);
                    id_type = DASH_TMPL_ID_UNDEFINED;
                }
            }
        }
    }
    return id_type;
}
