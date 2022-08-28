static int parse_inputs(const char **buf, AVFilterInOut **curr_inputs,
                        AVFilterInOut **open_outputs, void *log_ctx)
{
    AVFilterInOut *parsed_inputs = NULL;
    int pad = 0;
    while (**buf == '[') {
        char *name = parse_link_name(buf, log_ctx);
        AVFilterInOut *match;
        if (!name)
            return AVERROR(EINVAL);
        
        match = extract_inout(name, open_outputs);
        if (match) {
            av_free(name);
        } else {
            
            if (!(match = av_mallocz(sizeof(AVFilterInOut)))) {
                av_free(name);
                return AVERROR(ENOMEM);
            }
            match->name    = name;
            match->pad_idx = pad;
        }
        append_inout(&parsed_inputs, &match);
        *buf += strspn(*buf, WHITESPACES);
        pad++;
    }
    append_inout(&parsed_inputs, curr_inputs);
    *curr_inputs = parsed_inputs;
    return pad;
}
