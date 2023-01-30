static int parse_outputs(const char **buf, AVFilterInOut **curr_inputs,
                         AVFilterInOut **open_inputs,
                         AVFilterInOut **open_outputs, void *log_ctx)
{
    int ret, pad = 0;
    while (**buf == '[') {
        char *name = parse_link_name(buf, log_ctx);
        AVFilterInOut *match;
        AVFilterInOut *input = *curr_inputs;
        if (!name)
            return AVERROR(EINVAL);
        if (!input) {
            av_log(log_ctx, AV_LOG_ERROR,
                   "No output pad can be associated to link label '%s'.\n", name);
            av_free(name);
            return AVERROR(EINVAL);
        }
        *curr_inputs = (*curr_inputs)->next;
        
        match = extract_inout(name, open_inputs);
        if (match) {
            if ((ret = link_filter(input->filter_ctx, input->pad_idx,
                                   match->filter_ctx, match->pad_idx, log_ctx)) < 0) {
                av_free(name);
                return ret;
            }
            av_freep(&match->name);
            av_freep(&name);
            av_freep(&match);
            av_freep(&input);
        } else {
            
            input->name = name;
            insert_inout(open_outputs, input);
        }
        *buf += strspn(*buf, WHITESPACES);
        pad++;
    }
    return pad;
}
