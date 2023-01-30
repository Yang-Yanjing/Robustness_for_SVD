static void opt_list(void *obj, void *av_log_obj, const char *unit,
                     int req_flags, int rej_flags)
{
    const AVOption *opt=NULL;
    AVOptionRanges *r;
    int i;
    while ((opt = av_opt_next(obj, opt))) {
        if (!(opt->flags & req_flags) || (opt->flags & rej_flags))
            continue;
        



        if (!unit && opt->type==AV_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type!=AV_OPT_TYPE_CONST)
            continue;
        else if (unit && opt->type==AV_OPT_TYPE_CONST && strcmp(unit, opt->unit))
            continue;
        else if (unit && opt->type == AV_OPT_TYPE_CONST)
            av_log(av_log_obj, AV_LOG_INFO, "     %-15s ", opt->name);
        else
            av_log(av_log_obj, AV_LOG_INFO, "  %s%-17s ",
                   (opt->flags & AV_OPT_FLAG_FILTERING_PARAM) ? "" : "-",
                   opt->name);
        switch (opt->type) {
            case AV_OPT_TYPE_FLAGS:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<flags>");
                break;
            case AV_OPT_TYPE_INT:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<int>");
                break;
            case AV_OPT_TYPE_INT64:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<int64>");
                break;
            case AV_OPT_TYPE_DOUBLE:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<double>");
                break;
            case AV_OPT_TYPE_FLOAT:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<float>");
                break;
            case AV_OPT_TYPE_STRING:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<string>");
                break;
            case AV_OPT_TYPE_RATIONAL:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<rational>");
                break;
            case AV_OPT_TYPE_BINARY:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<binary>");
                break;
            case AV_OPT_TYPE_IMAGE_SIZE:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<image_size>");
                break;
            case AV_OPT_TYPE_VIDEO_RATE:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<video_rate>");
                break;
            case AV_OPT_TYPE_PIXEL_FMT:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<pix_fmt>");
                break;
            case AV_OPT_TYPE_SAMPLE_FMT:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<sample_fmt>");
                break;
            case AV_OPT_TYPE_DURATION:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<duration>");
                break;
            case AV_OPT_TYPE_COLOR:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<color>");
                break;
            case AV_OPT_TYPE_CHANNEL_LAYOUT:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "<channel_layout>");
                break;
            case AV_OPT_TYPE_CONST:
            default:
                av_log(av_log_obj, AV_LOG_INFO, "%-12s ", "");
                break;
        }
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_ENCODING_PARAM) ? 'E' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_DECODING_PARAM) ? 'D' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_FILTERING_PARAM)? 'F' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_VIDEO_PARAM   ) ? 'V' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_AUDIO_PARAM   ) ? 'A' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_SUBTITLE_PARAM) ? 'S' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_EXPORT)         ? 'X' : '.');
        av_log(av_log_obj, AV_LOG_INFO, "%c", (opt->flags & AV_OPT_FLAG_READONLY)       ? 'R' : '.');
        if (opt->help)
            av_log(av_log_obj, AV_LOG_INFO, " %s", opt->help);
        if (av_opt_query_ranges(&r, obj, opt->name, AV_OPT_SEARCH_FAKE_OBJ) >= 0) {
            switch (opt->type) {
            case AV_OPT_TYPE_INT:
            case AV_OPT_TYPE_INT64:
            case AV_OPT_TYPE_DOUBLE:
            case AV_OPT_TYPE_FLOAT:
            case AV_OPT_TYPE_RATIONAL:
                for (i = 0; i < r->nb_ranges; i++) {
                    av_log(av_log_obj, AV_LOG_INFO, " (from ");
                    log_value(av_log_obj, AV_LOG_INFO, r->range[i]->value_min);
                    av_log(av_log_obj, AV_LOG_INFO, " to ");
                    log_value(av_log_obj, AV_LOG_INFO, r->range[i]->value_max);
                    av_log(av_log_obj, AV_LOG_INFO, ")");
                }
                break;
            }
            av_opt_freep_ranges(&r);
        }
        if (opt->type != AV_OPT_TYPE_CONST  &&
            opt->type != AV_OPT_TYPE_BINARY &&
                !((opt->type == AV_OPT_TYPE_COLOR      ||
                   opt->type == AV_OPT_TYPE_IMAGE_SIZE ||
                   opt->type == AV_OPT_TYPE_STRING     ||
                   opt->type == AV_OPT_TYPE_VIDEO_RATE) &&
                  !opt->default_val.str)) {
            av_log(av_log_obj, AV_LOG_INFO, " (default ");
            switch (opt->type) {
            case AV_OPT_TYPE_FLAGS:
                av_log(av_log_obj, AV_LOG_INFO, "%"PRIX64, opt->default_val.i64);
                break;
            case AV_OPT_TYPE_DURATION:
            case AV_OPT_TYPE_INT:
            case AV_OPT_TYPE_INT64:
                log_value(av_log_obj, AV_LOG_INFO, opt->default_val.i64);
                break;
            case AV_OPT_TYPE_DOUBLE:
            case AV_OPT_TYPE_FLOAT:
                log_value(av_log_obj, AV_LOG_INFO, opt->default_val.dbl);
                break;
            case AV_OPT_TYPE_RATIONAL: {
                AVRational q = av_d2q(opt->default_val.dbl, INT_MAX);
                av_log(av_log_obj, AV_LOG_INFO, "%d/%d", q.num, q.den); }
                break;
            case AV_OPT_TYPE_PIXEL_FMT:
                av_log(av_log_obj, AV_LOG_INFO, "%s", (char *)av_x_if_null(av_get_pix_fmt_name(opt->default_val.i64), "none"));
                break;
            case AV_OPT_TYPE_SAMPLE_FMT:
                av_log(av_log_obj, AV_LOG_INFO, "%s", (char *)av_x_if_null(av_get_sample_fmt_name(opt->default_val.i64), "none"));
                break;
            case AV_OPT_TYPE_COLOR:
            case AV_OPT_TYPE_IMAGE_SIZE:
            case AV_OPT_TYPE_STRING:
            case AV_OPT_TYPE_VIDEO_RATE:
                av_log(av_log_obj, AV_LOG_INFO, "\"%s\"", opt->default_val.str);
                break;
            case AV_OPT_TYPE_CHANNEL_LAYOUT:
                av_log(av_log_obj, AV_LOG_INFO, "0x%"PRIx64, opt->default_val.i64);
                break;
            }
            av_log(av_log_obj, AV_LOG_INFO, ")");
        }
        av_log(av_log_obj, AV_LOG_INFO, "\n");
        if (opt->unit && opt->type != AV_OPT_TYPE_CONST) {
            opt_list(obj, av_log_obj, opt->unit, req_flags, rej_flags);
        }
    }
}
