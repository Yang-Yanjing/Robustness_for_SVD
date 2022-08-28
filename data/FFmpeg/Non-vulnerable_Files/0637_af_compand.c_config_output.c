static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx  = outlink->src;
    CompandContext *s     = ctx->priv;
    const int sample_rate = outlink->sample_rate;
    double radius         = s->curve_dB * M_LN10 / 20.0;
    char *p, *saveptr     = NULL;
    const int channels    = outlink->channels;
    int nb_attacks, nb_decays, nb_points;
    int new_nb_items, num;
    int i;
    int err;
    count_items(s->attacks, &nb_attacks);
    count_items(s->decays, &nb_decays);
    count_items(s->points, &nb_points);
    if (channels <= 0) {
        av_log(ctx, AV_LOG_ERROR, "Invalid number of channels: %d\n", channels);
        return AVERROR(EINVAL);
    }
    if (nb_attacks > channels || nb_decays > channels) {
        av_log(ctx, AV_LOG_ERROR,
                "Number of attacks/decays bigger than number of channels.\n");
        return AVERROR(EINVAL);
    }
    uninit(ctx);
    s->channels = av_mallocz_array(channels, sizeof(*s->channels));
    s->nb_segments = (nb_points + 4) * 2;
    s->segments = av_mallocz_array(s->nb_segments, sizeof(*s->segments));
    if (!s->channels || !s->segments) {
        uninit(ctx);
        return AVERROR(ENOMEM);
    }
    p = s->attacks;
    for (i = 0, new_nb_items = 0; i < nb_attacks; i++) {
        char *tstr = av_strtok(p, " |", &saveptr);
        p = NULL;
        new_nb_items += sscanf(tstr, "%lf", &s->channels[i].attack) == 1;
        if (s->channels[i].attack < 0) {
            uninit(ctx);
            return AVERROR(EINVAL);
        }
    }
    nb_attacks = new_nb_items;
    p = s->decays;
    for (i = 0, new_nb_items = 0; i < nb_decays; i++) {
        char *tstr = av_strtok(p, " |", &saveptr);
        p = NULL;
        new_nb_items += sscanf(tstr, "%lf", &s->channels[i].decay) == 1;
        if (s->channels[i].decay < 0) {
            uninit(ctx);
            return AVERROR(EINVAL);
        }
    }
    nb_decays = new_nb_items;
    if (nb_attacks != nb_decays) {
        av_log(ctx, AV_LOG_ERROR,
                "Number of attacks %d differs from number of decays %d.\n",
                nb_attacks, nb_decays);
        uninit(ctx);
        return AVERROR(EINVAL);
    }
    for (i = nb_decays; i < channels; i++) {
        s->channels[i].attack = s->channels[nb_decays - 1].attack;
        s->channels[i].decay = s->channels[nb_decays - 1].decay;
    }
#define S(x) s->segments[2 * ((x) + 1)]
    p = s->points;
    for (i = 0, new_nb_items = 0; i < nb_points; i++) {
        char *tstr = av_strtok(p, " |", &saveptr);
        p = NULL;
        if (sscanf(tstr, "%lf/%lf", &S(i).x, &S(i).y) != 2) {
            av_log(ctx, AV_LOG_ERROR,
                    "Invalid and/or missing input/output value.\n");
            uninit(ctx);
            return AVERROR(EINVAL);
        }
        if (i && S(i - 1).x > S(i).x) {
            av_log(ctx, AV_LOG_ERROR,
                    "Transfer function input values must be increasing.\n");
            uninit(ctx);
            return AVERROR(EINVAL);
        }
        S(i).y -= S(i).x;
        av_log(ctx, AV_LOG_DEBUG, "%d: x=%f y=%f\n", i, S(i).x, S(i).y);
        new_nb_items++;
    }
    num = new_nb_items;
    
    if (num == 0 || S(num - 1).x)
        num++;
#undef S
#define S(x) s->segments[2 * (x)]
    
    S(0).x = S(1).x - 2 * s->curve_dB;
    S(0).y = S(1).y;
    num++;
    
    for (i = 2; i < num; i++) {
        double g1 = (S(i - 1).y - S(i - 2).y) * (S(i - 0).x - S(i - 1).x);
        double g2 = (S(i - 0).y - S(i - 1).y) * (S(i - 1).x - S(i - 2).x);
        int j;
        if (fabs(g1 - g2))
            continue;
        num--;
        for (j = --i; j < num; j++)
            S(j) = S(j + 1);
    }
    for (i = 0; !i || s->segments[i - 2].x; i += 2) {
        s->segments[i].y += s->gain_dB;
        s->segments[i].x *= M_LN10 / 20;
        s->segments[i].y *= M_LN10 / 20;
    }
#define L(x) s->segments[i - (x)]
    for (i = 4; s->segments[i - 2].x; i += 2) {
        double x, y, cx, cy, in1, in2, out1, out2, theta, len, r;
        L(4).a = 0;
        L(4).b = (L(2).y - L(4).y) / (L(2).x - L(4).x);
        L(2).a = 0;
        L(2).b = (L(0).y - L(2).y) / (L(0).x - L(2).x);
        theta = atan2(L(2).y - L(4).y, L(2).x - L(4).x);
        len = sqrt(pow(L(2).x - L(4).x, 2.) + pow(L(2).y - L(4).y, 2.));
        r = FFMIN(radius, len);
        L(3).x = L(2).x - r * cos(theta);
        L(3).y = L(2).y - r * sin(theta);
        theta = atan2(L(0).y - L(2).y, L(0).x - L(2).x);
        len = sqrt(pow(L(0).x - L(2).x, 2.) + pow(L(0).y - L(2).y, 2.));
        r = FFMIN(radius, len / 2);
        x = L(2).x + r * cos(theta);
        y = L(2).y + r * sin(theta);
        cx = (L(3).x + L(2).x + x) / 3;
        cy = (L(3).y + L(2).y + y) / 3;
        L(2).x = x;
        L(2).y = y;
        in1  = cx - L(3).x;
        out1 = cy - L(3).y;
        in2  = L(2).x - L(3).x;
        out2 = L(2).y - L(3).y;
        L(3).a = (out2 / in2 - out1 / in1) / (in2 - in1);
        L(3).b = out1 / in1 - L(3).a * in1;
    }
    L(3).x = 0;
    L(3).y = L(2).y;
    s->in_min_lin  = exp(s->segments[1].x);
    s->out_min_lin = exp(s->segments[1].y);
    for (i = 0; i < channels; i++) {
        ChanParam *cp = &s->channels[i];
        if (cp->attack > 1.0 / sample_rate)
            cp->attack = 1.0 - exp(-1.0 / (sample_rate * cp->attack));
        else
            cp->attack = 1.0;
        if (cp->decay > 1.0 / sample_rate)
            cp->decay = 1.0 - exp(-1.0 / (sample_rate * cp->decay));
        else
            cp->decay = 1.0;
        cp->volume = pow(10.0, s->initial_volume / 20);
    }
    s->delay_samples = s->delay * sample_rate;
    if (s->delay_samples <= 0) {
        s->compand = compand_nodelay;
        return 0;
    }
    s->delay_frame = av_frame_alloc();
    if (!s->delay_frame) {
        uninit(ctx);
        return AVERROR(ENOMEM);
    }
    s->delay_frame->format         = outlink->format;
    s->delay_frame->nb_samples     = s->delay_samples;
    s->delay_frame->channel_layout = outlink->channel_layout;
    err = av_frame_get_buffer(s->delay_frame, 32);
    if (err)
        return err;
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    s->compand = compand_delay;
    return 0;
}
