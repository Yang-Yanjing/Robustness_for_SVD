static int config_props(AVFilterLink *link)
{
    AVFilterContext *ctx = link->dst;
    PanContext *pan = ctx->priv;
    char buf[1024], *cur;
    int i, j, k, r;
    double t;
    if (pan->need_renumber) {
        
        for (i = j = 0; i < MAX_CHANNELS; i++) {
            if ((link->channel_layout >> i) & 1) {
                for (k = 0; k < pan->nb_output_channels; k++)
                    pan->gain[k][j] = pan->gain[k][i];
                j++;
            }
        }
    }
    
    
    if (link->channels > MAX_CHANNELS ||
        pan->nb_output_channels > MAX_CHANNELS) {
        av_log(ctx, AV_LOG_ERROR,
               "af_pan support a maximum of %d channels. "
               "Feel free to ask for a higher limit.\n", MAX_CHANNELS);
        return AVERROR_PATCHWELCOME;
    }
    
    pan->swr = swr_alloc_set_opts(pan->swr,
                                  pan->out_channel_layout, link->format, link->sample_rate,
                                  link->channel_layout,    link->format, link->sample_rate,
                                  0, ctx);
    if (!pan->swr)
        return AVERROR(ENOMEM);
    if (!link->channel_layout) {
        if (av_opt_set_int(pan->swr, "ich", link->channels, 0) < 0)
            return AVERROR(EINVAL);
    }
    if (!pan->out_channel_layout) {
        if (av_opt_set_int(pan->swr, "och", pan->nb_output_channels, 0) < 0)
            return AVERROR(EINVAL);
    }
    
    if (pan->pure_gains) {
        
        for (i = 0; i < pan->nb_output_channels; i++) {
            int ch_id = -1;
            for (j = 0; j < link->channels; j++) {
                if (pan->gain[i][j]) {
                    ch_id = j;
                    break;
                }
            }
            pan->channel_map[i] = ch_id;
        }
        av_opt_set_int(pan->swr, "icl", pan->out_channel_layout, 0);
        av_opt_set_int(pan->swr, "uch", pan->nb_output_channels, 0);
        swr_set_channel_mapping(pan->swr, pan->channel_map);
    } else {
        
        for (i = 0; i < pan->nb_output_channels; i++) {
            if (!((pan->need_renorm >> i) & 1))
                continue;
            t = 0;
            for (j = 0; j < link->channels; j++)
                t += pan->gain[i][j];
            if (t > -1E-5 && t < 1E-5) {
                
                if (t)
                    av_log(ctx, AV_LOG_WARNING,
                           "Degenerate coefficients while renormalizing\n");
                continue;
            }
            for (j = 0; j < link->channels; j++)
                pan->gain[i][j] /= t;
        }
        av_opt_set_int(pan->swr, "icl", link->channel_layout, 0);
        av_opt_set_int(pan->swr, "ocl", pan->out_channel_layout, 0);
        swr_set_matrix(pan->swr, pan->gain[0], pan->gain[1] - pan->gain[0]);
    }
    r = swr_init(pan->swr);
    if (r < 0)
        return r;
    
    for (i = 0; i < pan->nb_output_channels; i++) {
        cur = buf;
        for (j = 0; j < link->channels; j++) {
            r = snprintf(cur, buf + sizeof(buf) - cur, "%s%.3g i%d",
                         j ? " + " : "", pan->gain[i][j], j);
            cur += FFMIN(buf + sizeof(buf) - cur, r);
        }
        av_log(ctx, AV_LOG_VERBOSE, "o%d = %s\n", i, buf);
    }
    
    if (pan->pure_gains) {
        av_log(ctx, AV_LOG_INFO, "Pure channel mapping detected:");
        for (i = 0; i < pan->nb_output_channels; i++)
            if (pan->channel_map[i] < 0)
                av_log(ctx, AV_LOG_INFO, " M");
            else
                av_log(ctx, AV_LOG_INFO, " %d", pan->channel_map[i]);
        av_log(ctx, AV_LOG_INFO, "\n");
        return 0;
    }
    return 0;
}
