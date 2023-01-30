static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    ShowCQTContext *s = ctx->priv;
    AVExpr *tlength_expr = NULL, *volume_expr = NULL, *fontcolor_expr = NULL;
    uint8_t *fontcolor_value = s->fontcolor_value;
    static const char * const expr_vars[] = { "timeclamp", "tc", "frequency", "freq", "f", NULL };
    static const char * const expr_func_names[] = { "a_weighting", "b_weighting", "c_weighting", NULL };
    static const char * const expr_fontcolor_func_names[] = { "midi", "r", "g", "b", NULL };
    static double (* const expr_funcs[])(void *, double) = { a_weighting, b_weighting, c_weighting, NULL };
    static double (* const expr_fontcolor_funcs[])(void *, double) = { midi, r_func, g_func, b_func, NULL };
    int fft_len, k, x, y, ret;
    int num_coeffs = 0;
    int rate = inlink->sample_rate;
    double max_len = rate * (double) s->timeclamp;
    int64_t start_time, end_time;
    int video_scale = s->fullhd ? 2 : 1;
    int video_width = (VIDEO_WIDTH/2) * video_scale;
    int video_height = (VIDEO_HEIGHT/2) * video_scale;
    int spectogram_height = (SPECTOGRAM_HEIGHT/2) * video_scale;
    s->fft_bits = ceil(log2(max_len));
    fft_len = 1 << s->fft_bits;
    if (rate % (s->fps * s->count)) {
        av_log(ctx, AV_LOG_ERROR, "Rate (%u) is not divisible by fps*count (%u*%u)\n", rate, s->fps, s->count);
        return AVERROR(EINVAL);
    }
    s->fft_data         = av_malloc_array(fft_len, sizeof(*s->fft_data));
    s->coeff_sort       = av_malloc_array(fft_len, sizeof(*s->coeff_sort));
    s->fft_result_left  = av_malloc_array(fft_len, sizeof(*s->fft_result_left));
    s->fft_result_right = av_malloc_array(fft_len, sizeof(*s->fft_result_right));
    s->fft_context      = av_fft_init(s->fft_bits, 0);
    if (!s->fft_data || !s->coeff_sort || !s->fft_result_left || !s->fft_result_right || !s->fft_context)
        return AVERROR(ENOMEM);
#if CONFIG_LIBFREETYPE
    load_freetype_font(ctx);
#else
    if (s->fontfile)
        av_log(ctx, AV_LOG_WARNING, "Freetype is not available, ignoring fontfile option\n");
    s->font_alpha = NULL;
#endif
    av_log(ctx, AV_LOG_INFO, "Calculating spectral kernel, please wait\n");
    start_time = av_gettime_relative();
    ret = av_expr_parse(&tlength_expr, s->tlength, expr_vars, NULL, NULL, NULL, NULL, 0, ctx);
    if (ret < 0)
        goto eval_error;
    ret = av_expr_parse(&volume_expr, s->volume, expr_vars, expr_func_names,
                        expr_funcs, NULL, NULL, 0, ctx);
    if (ret < 0)
        goto eval_error;
    ret = av_expr_parse(&fontcolor_expr, s->fontcolor, expr_vars, expr_fontcolor_func_names,
                        expr_fontcolor_funcs, NULL, NULL, 0, ctx);
    if (ret < 0)
        goto eval_error;
    for (k = 0; k < VIDEO_WIDTH; k++) {
        int hlen = fft_len >> 1;
        float total = 0;
        float partial = 0;
        double freq = BASE_FREQ * exp2(k * (1.0/192.0));
        double tlen, tlength, volume;
        double expr_vars_val[] = { s->timeclamp, s->timeclamp, freq, freq, freq, 0 };
        



        double a0 = 0.355768;
        double a1 = 0.487396/a0;
        double a2 = 0.144232/a0;
        double a3 = 0.012604/a0;
        double sv_step, cv_step, sv, cv;
        double sw_step, cw_step, sw, cw, w;
        tlength = av_expr_eval(tlength_expr, expr_vars_val, NULL);
        if (isnan(tlength)) {
            av_log(ctx, AV_LOG_WARNING, "at freq %g: tlength is nan, setting it to %g\n", freq, s->timeclamp);
            tlength = s->timeclamp;
        } else if (tlength < TLENGTH_MIN) {
            av_log(ctx, AV_LOG_WARNING, "at freq %g: tlength is %g, setting it to %g\n", freq, tlength, TLENGTH_MIN);
            tlength = TLENGTH_MIN;
        } else if (tlength > s->timeclamp) {
            av_log(ctx, AV_LOG_WARNING, "at freq %g: tlength is %g, setting it to %g\n", freq, tlength, s->timeclamp);
            tlength = s->timeclamp;
        }
        volume = FFABS(av_expr_eval(volume_expr, expr_vars_val, NULL));
        if (isnan(volume)) {
            av_log(ctx, AV_LOG_WARNING, "at freq %g: volume is nan, setting it to 0\n", freq);
            volume = VOLUME_MIN;
        } else if (volume < VOLUME_MIN) {
            volume = VOLUME_MIN;
        } else if (volume > VOLUME_MAX) {
            av_log(ctx, AV_LOG_WARNING, "at freq %g: volume is %g, setting it to %g\n", freq, volume, VOLUME_MAX);
            volume = VOLUME_MAX;
        }
        if (s->fullhd || !(k & 1)) {
            int fontcolor = av_expr_eval(fontcolor_expr, expr_vars_val, NULL);
            fontcolor_value[0] = (fontcolor >> 16) & 0xFF;
            fontcolor_value[1] = (fontcolor >> 8) & 0xFF;
            fontcolor_value[2] = fontcolor & 0xFF;
            fontcolor_value += 3;
        }
        tlen = tlength * rate;
        s->fft_data[0].re = 0;
        s->fft_data[0].im = 0;
        s->fft_data[hlen].re = (1.0 + a1 + a2 + a3) * (1.0/tlen) * volume * (1.0/fft_len);
        s->fft_data[hlen].im = 0;
        sv_step = sv = sin(2.0*M_PI*freq*(1.0/rate));
        cv_step = cv = cos(2.0*M_PI*freq*(1.0/rate));
        
        sw_step = sw = sin(2.0*M_PI*(1.0/tlen));
        cw_step = cw = cos(2.0*M_PI*(1.0/tlen));
        for (x = 1; x < 0.5 * tlen; x++) {
            double cv_tmp, cw_tmp;
            double cw2, cw3, sw2;
            cw2 = cw * cw - sw * sw;
            sw2 = cw * sw + sw * cw;
            cw3 = cw * cw2 - sw * sw2;
            w = (1.0 + a1 * cw + a2 * cw2 + a3 * cw3) * (1.0/tlen) * volume * (1.0/fft_len);
            s->fft_data[hlen + x].re = w * cv;
            s->fft_data[hlen + x].im = w * sv;
            s->fft_data[hlen - x].re = s->fft_data[hlen + x].re;
            s->fft_data[hlen - x].im = -s->fft_data[hlen + x].im;
            cv_tmp = cv * cv_step - sv * sv_step;
            sv = sv * cv_step + cv * sv_step;
            cv = cv_tmp;
            cw_tmp = cw * cw_step - sw * sw_step;
            sw = sw * cw_step + cw * sw_step;
            cw = cw_tmp;
        }
        for (; x < hlen; x++) {
            s->fft_data[hlen + x].re = 0;
            s->fft_data[hlen + x].im = 0;
            s->fft_data[hlen - x].re = 0;
            s->fft_data[hlen - x].im = 0;
        }
        av_fft_permute(s->fft_context, s->fft_data);
        av_fft_calc(s->fft_context, s->fft_data);
        for (x = 0; x < fft_len; x++) {
            s->coeff_sort[x].index = x;
            s->coeff_sort[x].value = s->fft_data[x].re;
        }
        AV_QSORT(s->coeff_sort, fft_len, SparseCoeff, qsort_sparsecoeff);
        for (x = 0; x < fft_len; x++)
            total += fabsf(s->coeff_sort[x].value);
        for (x = 0; x < fft_len; x++) {
            partial += fabsf(s->coeff_sort[x].value);
            if (partial > total * s->coeffclamp * COEFF_CLAMP) {
                s->coeffs_len[k] = fft_len - x;
                num_coeffs += s->coeffs_len[k];
                s->coeffs[k] = av_malloc_array(s->coeffs_len[k], sizeof(*s->coeffs[k]));
                if (!s->coeffs[k]) {
                    ret = AVERROR(ENOMEM);
                    goto eval_error;
                }
                for (y = 0; y < s->coeffs_len[k]; y++)
                    s->coeffs[k][y] = s->coeff_sort[x+y];
                break;
            }
        }
    }
    av_expr_free(fontcolor_expr);
    av_expr_free(volume_expr);
    av_expr_free(tlength_expr);
    end_time = av_gettime_relative();
    av_log(ctx, AV_LOG_INFO, "Elapsed time %.6f s (fft_len=%u, num_coeffs=%u)\n", 1e-6 * (end_time-start_time), fft_len, num_coeffs);
    outlink->w = video_width;
    outlink->h = video_height;
    s->req_fullfilled = 0;
    s->spectogram_index = 0;
    s->frame_count = 0;
    s->spectogram_count = 0;
    s->remaining_fill = fft_len >> 1;
    memset(s->fft_data, 0, fft_len * sizeof(*s->fft_data));
    s->outpicref = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!s->outpicref)
        return AVERROR(ENOMEM);
    s->spectogram = av_calloc(spectogram_height, s->outpicref->linesize[0]);
    if (!s->spectogram)
        return AVERROR(ENOMEM);
    outlink->sample_aspect_ratio = av_make_q(1, 1);
    outlink->time_base = av_make_q(1, s->fps);
    outlink->frame_rate = av_make_q(s->fps, 1);
    return 0;
eval_error:
    av_expr_free(fontcolor_expr);
    av_expr_free(volume_expr);
    av_expr_free(tlength_expr);
    return ret;
}
