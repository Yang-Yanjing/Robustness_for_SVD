static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    int i, ch, idx_insample;
    AVFilterContext *ctx = inlink->dst;
    EBUR128Context *ebur128 = ctx->priv;
    const int nb_channels = ebur128->nb_channels;
    const int nb_samples  = insamples->nb_samples;
    const double *samples = (double *)insamples->data[0];
    AVFrame *pic = ebur128->outpicref;
#if CONFIG_SWRESAMPLE
    if (ebur128->peak_mode & PEAK_MODE_TRUE_PEAKS) {
        const double *swr_samples = ebur128->swr_buf;
        int ret = swr_convert(ebur128->swr_ctx, (uint8_t**)&ebur128->swr_buf, 19200,
                              (const uint8_t **)insamples->data, nb_samples);
        if (ret < 0)
            return ret;
        for (ch = 0; ch < nb_channels; ch++)
            ebur128->true_peaks_per_frame[ch] = 0.0;
        for (idx_insample = 0; idx_insample < ret; idx_insample++) {
            for (ch = 0; ch < nb_channels; ch++) {
                ebur128->true_peaks[ch] = FFMAX(ebur128->true_peaks[ch], FFABS(*swr_samples));
                ebur128->true_peaks_per_frame[ch] = FFMAX(ebur128->true_peaks_per_frame[ch],
                                                          FFABS(*swr_samples));
                swr_samples++;
            }
        }
    }
#endif
    for (idx_insample = 0; idx_insample < nb_samples; idx_insample++) {
        const int bin_id_400  = ebur128->i400.cache_pos;
        const int bin_id_3000 = ebur128->i3000.cache_pos;
#define MOVE_TO_NEXT_CACHED_ENTRY(time) do {                \
    ebur128->i##time.cache_pos++;                           \
    if (ebur128->i##time.cache_pos == I##time##_BINS) {     \
        ebur128->i##time.filled    = 1;                     \
        ebur128->i##time.cache_pos = 0;                     \
    }                                                       \
} while (0)
        MOVE_TO_NEXT_CACHED_ENTRY(400);
        MOVE_TO_NEXT_CACHED_ENTRY(3000);
        for (ch = 0; ch < nb_channels; ch++) {
            double bin;
            if (ebur128->peak_mode & PEAK_MODE_SAMPLES_PEAKS)
                ebur128->sample_peaks[ch] = FFMAX(ebur128->sample_peaks[ch], FFABS(*samples));
            ebur128->x[ch * 3] = *samples++; 
            if (!ebur128->ch_weighting[ch])
                continue;
            
#define FILTER(Y, X, name) do {                                                 \
            double *dst = ebur128->Y + ch*3;                                    \
            double *src = ebur128->X + ch*3;                                    \
            dst[2] = dst[1];                                                    \
            dst[1] = dst[0];                                                    \
            dst[0] = src[0]*name##_B0 + src[1]*name##_B1 + src[2]*name##_B2     \
                                      - dst[1]*name##_A1 - dst[2]*name##_A2;    \
} while (0)
            
            FILTER(y, x, PRE);  
            ebur128->x[ch * 3 + 2] = ebur128->x[ch * 3 + 1];
            ebur128->x[ch * 3 + 1] = ebur128->x[ch * 3    ];
            FILTER(z, y, RLB);  
            bin = ebur128->z[ch * 3] * ebur128->z[ch * 3];
            

            ebur128->i400.sum [ch] = ebur128->i400.sum [ch] + bin - ebur128->i400.cache [ch][bin_id_400];
            ebur128->i3000.sum[ch] = ebur128->i3000.sum[ch] + bin - ebur128->i3000.cache[ch][bin_id_3000];
            
            ebur128->i400.cache [ch][bin_id_400 ] = bin;
            ebur128->i3000.cache[ch][bin_id_3000] = bin;
        }
        


        if (++ebur128->sample_count == 4800) {
            double loudness_400, loudness_3000;
            double power_400 = 1e-12, power_3000 = 1e-12;
            AVFilterLink *outlink = ctx->outputs[0];
            const int64_t pts = insamples->pts +
                av_rescale_q(idx_insample, (AVRational){ 1, inlink->sample_rate },
                             outlink->time_base);
            ebur128->sample_count = 0;
#define COMPUTE_LOUDNESS(m, time) do {                                              \
    if (ebur128->i##time.filled) {                                                  \
                                           \
        for (ch = 0; ch < nb_channels; ch++)                                        \
            power_##time += ebur128->ch_weighting[ch] * ebur128->i##time.sum[ch];   \
        power_##time /= I##time##_BINS;                                             \
    }                                                                               \
    loudness_##time = LOUDNESS(power_##time);                                       \
} while (0)
            COMPUTE_LOUDNESS(M,  400);
            COMPUTE_LOUDNESS(S, 3000);
            
#define I_GATE_THRES -10  
            if (loudness_400 >= ABS_THRES) {
                double integrated_sum = 0;
                int nb_integrated = 0;
                int gate_hist_pos = gate_update(&ebur128->i400, power_400,
                                                loudness_400, I_GATE_THRES);
                

                for (i = gate_hist_pos; i < HIST_SIZE; i++) {
                    const int nb_v = ebur128->i400.histogram[i].count;
                    nb_integrated  += nb_v;
                    integrated_sum += nb_v * ebur128->i400.histogram[i].energy;
                }
                if (nb_integrated)
                    ebur128->integrated_loudness = LOUDNESS(integrated_sum / nb_integrated);
            }
            
#define LRA_GATE_THRES -20
#define LRA_LOWER_PRC   10
#define LRA_HIGHER_PRC  95
            

            if (loudness_3000 >= ABS_THRES) {
                int nb_powers = 0;
                int gate_hist_pos = gate_update(&ebur128->i3000, power_3000,
                                                loudness_3000, LRA_GATE_THRES);
                for (i = gate_hist_pos; i < HIST_SIZE; i++)
                    nb_powers += ebur128->i3000.histogram[i].count;
                if (nb_powers) {
                    int n, nb_pow;
                    
                    n = 0;
                    nb_pow = LRA_LOWER_PRC  * nb_powers / 100. + 0.5;
                    for (i = gate_hist_pos; i < HIST_SIZE; i++) {
                        n += ebur128->i3000.histogram[i].count;
                        if (n >= nb_pow) {
                            ebur128->lra_low = ebur128->i3000.histogram[i].loudness;
                            break;
                        }
                    }
                    
                    n = nb_powers;
                    nb_pow = LRA_HIGHER_PRC * nb_powers / 100. + 0.5;
                    for (i = HIST_SIZE - 1; i >= 0; i--) {
                        n -= ebur128->i3000.histogram[i].count;
                        if (n < nb_pow) {
                            ebur128->lra_high = ebur128->i3000.histogram[i].loudness;
                            break;
                        }
                    }
                    
                    ebur128->loudness_range = ebur128->lra_high - ebur128->lra_low;
                }
            }
#define LOG_FMT "M:%6.1f S:%6.1f     I:%6.1f LUFS     LRA:%6.1f LU"
            
            if (ebur128->do_video) {
                int x, y, ret;
                uint8_t *p;
                const int y_loudness_lu_graph = lu_to_y(ebur128, loudness_3000 + 23);
                const int y_loudness_lu_gauge = lu_to_y(ebur128, loudness_400  + 23);
                
                p = pic->data[0] + ebur128->graph.y*pic->linesize[0] + ebur128->graph.x*3;
                for (y = 0; y < ebur128->graph.h; y++) {
                    const uint8_t *c = get_graph_color(ebur128, y_loudness_lu_graph, y);
                    memmove(p, p + 3, (ebur128->graph.w - 1) * 3);
                    memcpy(p + (ebur128->graph.w - 1) * 3, c, 3);
                    p += pic->linesize[0];
                }
                
                p = pic->data[0] + ebur128->gauge.y*pic->linesize[0] + ebur128->gauge.x*3;
                for (y = 0; y < ebur128->gauge.h; y++) {
                    const uint8_t *c = get_graph_color(ebur128, y_loudness_lu_gauge, y);
                    for (x = 0; x < ebur128->gauge.w; x++)
                        memcpy(p + x*3, c, 3);
                    p += pic->linesize[0];
                }
                
                drawtext(pic, PAD, PAD - PAD/2, FONT16, font_colors,
                         LOG_FMT "     ", 
                         loudness_400, loudness_3000,
                         ebur128->integrated_loudness, ebur128->loudness_range);
                
                pic->pts = pts;
                ret = ff_filter_frame(outlink, av_frame_clone(pic));
                if (ret < 0)
                    return ret;
            }
            if (ebur128->metadata) { 
                char metabuf[128];
#define META_PREFIX "lavfi.r128."
#define SET_META(name, var) do {                                            \
    snprintf(metabuf, sizeof(metabuf), "%.3f", var);                        \
    av_dict_set(&insamples->metadata, name, metabuf, 0);                    \
} while (0)
#define SET_META_PEAK(name, ptype) do {                                     \
    if (ebur128->peak_mode & PEAK_MODE_ ## ptype ## _PEAKS) {               \
        char key[64];                                                       \
        for (ch = 0; ch < nb_channels; ch++) {                              \
            snprintf(key, sizeof(key),                                      \
                     META_PREFIX AV_STRINGIFY(name) "_peaks_ch%d", ch);     \
            SET_META(key, ebur128->name##_peaks[ch]);                       \
        }                                                                   \
    }                                                                       \
} while (0)
                SET_META(META_PREFIX "M",        loudness_400);
                SET_META(META_PREFIX "S",        loudness_3000);
                SET_META(META_PREFIX "I",        ebur128->integrated_loudness);
                SET_META(META_PREFIX "LRA",      ebur128->loudness_range);
                SET_META(META_PREFIX "LRA.low",  ebur128->lra_low);
                SET_META(META_PREFIX "LRA.high", ebur128->lra_high);
                SET_META_PEAK(sample, SAMPLES);
                SET_META_PEAK(true,   TRUE);
            }
            av_log(ctx, ebur128->loglevel, "t: %-10s " LOG_FMT,
                   av_ts2timestr(pts, &outlink->time_base),
                   loudness_400, loudness_3000,
                   ebur128->integrated_loudness, ebur128->loudness_range);
#define PRINT_PEAKS(str, sp, ptype) do {                            \
    if (ebur128->peak_mode & PEAK_MODE_ ## ptype ## _PEAKS) {       \
        av_log(ctx, ebur128->loglevel, "  " str ":");               \
        for (ch = 0; ch < nb_channels; ch++)                        \
            av_log(ctx, ebur128->loglevel, " %5.1f", DBFS(sp[ch])); \
        av_log(ctx, ebur128->loglevel, " dBFS");                    \
    }                                                               \
} while (0)
            PRINT_PEAKS("SPK", ebur128->sample_peaks, SAMPLES);
            PRINT_PEAKS("FTPK", ebur128->true_peaks_per_frame, TRUE);
            PRINT_PEAKS("TPK", ebur128->true_peaks,   TRUE);
            av_log(ctx, ebur128->loglevel, "\n");
        }
    }
    return ff_filter_frame(ctx->outputs[ebur128->do_video], insamples);
}
