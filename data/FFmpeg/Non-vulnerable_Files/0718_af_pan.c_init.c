static av_cold int init(AVFilterContext *ctx)
{
    PanContext *const pan = ctx->priv;
    char *arg, *arg0, *tokenizer, *args = av_strdup(pan->args);
    int out_ch_id, in_ch_id, len, named, ret;
    int nb_in_channels[2] = { 0, 0 }; 
    double gain;
    if (!pan->args) {
        av_log(ctx, AV_LOG_ERROR,
               "pan filter needs a channel layout and a set "
               "of channels definitions as parameter\n");
        return AVERROR(EINVAL);
    }
    if (!args)
        return AVERROR(ENOMEM);
    arg = av_strtok(args, "|", &tokenizer);
    ret = ff_parse_channel_layout(&pan->out_channel_layout,
                                  &pan->nb_output_channels, arg, ctx);
    if (ret < 0)
        goto fail;
    
    while ((arg = arg0 = av_strtok(NULL, "|", &tokenizer))) {
        
        if (parse_channel_name(&arg, &out_ch_id, &named)) {
            av_log(ctx, AV_LOG_ERROR,
                   "Expected out channel name, got \"%.8s\"\n", arg);
            ret = AVERROR(EINVAL);
            goto fail;
        }
        if (named) {
            if (!((pan->out_channel_layout >> out_ch_id) & 1)) {
                av_log(ctx, AV_LOG_ERROR,
                       "Channel \"%.8s\" does not exist in the chosen layout\n", arg0);
                ret = AVERROR(EINVAL);
                goto fail;
            }
            



            out_ch_id = av_get_channel_layout_nb_channels(pan->out_channel_layout & (((int64_t)1 << out_ch_id) - 1));
        }
        if (out_ch_id < 0 || out_ch_id >= pan->nb_output_channels) {
            av_log(ctx, AV_LOG_ERROR,
                   "Invalid out channel name \"%.8s\"\n", arg0);
            ret = AVERROR(EINVAL);
            goto fail;
        }
        skip_spaces(&arg);
        if (*arg == '=') {
            arg++;
        } else if (*arg == '<') {
            pan->need_renorm |= (int64_t)1 << out_ch_id;
            arg++;
        } else {
            av_log(ctx, AV_LOG_ERROR,
                   "Syntax error after channel name in \"%.8s\"\n", arg0);
            ret = AVERROR(EINVAL);
            goto fail;
        }
        
        while (1) {
            gain = 1;
            if (sscanf(arg, "%lf%n *%n", &gain, &len, &len))
                arg += len;
            if (parse_channel_name(&arg, &in_ch_id, &named)){
                av_log(ctx, AV_LOG_ERROR,
                       "Expected in channel name, got \"%.8s\"\n", arg);
                 ret = AVERROR(EINVAL);
                 goto fail;
            }
            nb_in_channels[named]++;
            if (nb_in_channels[!named]) {
                av_log(ctx, AV_LOG_ERROR,
                       "Can not mix named and numbered channels\n");
                ret = AVERROR(EINVAL);
                goto fail;
            }
            pan->gain[out_ch_id][in_ch_id] = gain;
            skip_spaces(&arg);
            if (!*arg)
                break;
            if (*arg != '+') {
                av_log(ctx, AV_LOG_ERROR, "Syntax error near \"%.8s\"\n", arg);
                ret = AVERROR(EINVAL);
                goto fail;
            }
            arg++;
        }
    }
    pan->need_renumber = !!nb_in_channels[1];
    ret = 0;
fail:
    av_free(args);
    return ret;
}
