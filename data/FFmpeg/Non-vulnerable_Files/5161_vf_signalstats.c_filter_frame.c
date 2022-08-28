static int filter_frame(AVFilterLink *link, AVFrame *in)
{
    AVFilterContext *ctx = link->dst;
    SignalstatsContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out = in;
    int i, j;
    int  w = 0,  cw = 0, 
        pw = 0, cpw = 0; 
    int fil;
    char metabuf[128];
    unsigned int histy[DEPTH] = {0},
                 histu[DEPTH] = {0},
                 histv[DEPTH] = {0},
                 histhue[360] = {0},
                 histsat[DEPTH] = {0}; 
    int miny  = -1, minu  = -1, minv  = -1;
    int maxy  = -1, maxu  = -1, maxv  = -1;
    int lowy  = -1, lowu  = -1, lowv  = -1;
    int highy = -1, highu = -1, highv = -1;
    int minsat = -1, maxsat = -1, lowsat = -1, highsat = -1;
    int lowp, highp, clowp, chighp;
    int accy, accu, accv;
    int accsat, acchue = 0;
    int medhue, maxhue;
    int toty = 0, totu = 0, totv = 0, totsat=0;
    int tothue = 0;
    int dify = 0, difu = 0, difv = 0;
    int filtot[FILT_NUMB] = {0};
    AVFrame *prev;
    AVFrame *sat = s->frame_sat;
    AVFrame *hue = s->frame_hue;
    const uint8_t *p_sat = sat->data[0];
    const uint8_t *p_hue = hue->data[0];
    const int lsz_sat = sat->linesize[0];
    const int lsz_hue = hue->linesize[0];
    ThreadDataHueSatMetrics td_huesat = {
        .src     = in,
        .dst_sat = sat,
        .dst_hue = hue,
    };
    if (!s->frame_prev)
        s->frame_prev = av_frame_clone(in);
    prev = s->frame_prev;
    if (s->outfilter != FILTER_NONE) {
        out = av_frame_clone(in);
        av_frame_make_writable(out);
    }
    ctx->internal->execute(ctx, compute_sat_hue_metrics, &td_huesat,
                           NULL, FFMIN(s->chromah, ctx->graph->nb_threads));
    
    for (j = 0; j < link->h; j++) {
        for (i = 0; i < link->w; i++) {
            const int yuv = in->data[0][w + i];
            histy[yuv]++;
            dify += abs(yuv - prev->data[0][pw + i]);
        }
        w  += in->linesize[0];
        pw += prev->linesize[0];
    }
    
    for (j = 0; j < s->chromah; j++) {
        for (i = 0; i < s->chromaw; i++) {
            const int yuvu = in->data[1][cw+i];
            const int yuvv = in->data[2][cw+i];
            histu[yuvu]++;
            difu += abs(yuvu - prev->data[1][cpw+i]);
            histv[yuvv]++;
            difv += abs(yuvv - prev->data[2][cpw+i]);
            histsat[p_sat[i]]++;
            histhue[((int16_t*)p_hue)[i]]++;
        }
        cw  += in->linesize[1];
        cpw += prev->linesize[1];
        p_sat += lsz_sat;
        p_hue += lsz_hue;
    }
    for (fil = 0; fil < FILT_NUMB; fil ++) {
        if (s->filters & 1<<fil) {
            ThreadData td = {
                .in = in,
                .out = out != in && s->outfilter == fil ? out : NULL,
            };
            memset(s->jobs_rets, 0, s->nb_jobs * sizeof(*s->jobs_rets));
            ctx->internal->execute(ctx, filters_def[fil].process,
                                   &td, s->jobs_rets, s->nb_jobs);
            for (i = 0; i < s->nb_jobs; i++)
                filtot[fil] += s->jobs_rets[i];
        }
    }
    
    
    lowp   = lrint(s->fs  * 10 / 100.);
    highp  = lrint(s->fs  * 90 / 100.);
    clowp  = lrint(s->cfs * 10 / 100.);
    chighp = lrint(s->cfs * 90 / 100.);
    accy = accu = accv = accsat = 0;
    for (fil = 0; fil < DEPTH; fil++) {
        if (miny   < 0 && histy[fil])   miny = fil;
        if (minu   < 0 && histu[fil])   minu = fil;
        if (minv   < 0 && histv[fil])   minv = fil;
        if (minsat < 0 && histsat[fil]) minsat = fil;
        if (histy[fil])   maxy   = fil;
        if (histu[fil])   maxu   = fil;
        if (histv[fil])   maxv   = fil;
        if (histsat[fil]) maxsat = fil;
        toty   += histy[fil]   * fil;
        totu   += histu[fil]   * fil;
        totv   += histv[fil]   * fil;
        totsat += histsat[fil] * fil;
        accy   += histy[fil];
        accu   += histu[fil];
        accv   += histv[fil];
        accsat += histsat[fil];
        if (lowy   == -1 && accy   >=  lowp) lowy   = fil;
        if (lowu   == -1 && accu   >= clowp) lowu   = fil;
        if (lowv   == -1 && accv   >= clowp) lowv   = fil;
        if (lowsat == -1 && accsat >= clowp) lowsat = fil;
        if (highy   == -1 && accy   >=  highp) highy   = fil;
        if (highu   == -1 && accu   >= chighp) highu   = fil;
        if (highv   == -1 && accv   >= chighp) highv   = fil;
        if (highsat == -1 && accsat >= chighp) highsat = fil;
    }
    maxhue = histhue[0];
    medhue = -1;
    for (fil = 0; fil < 360; fil++) {
        tothue += histhue[fil] * fil;
        acchue += histhue[fil];
        if (medhue == -1 && acchue > s->cfs / 2)
            medhue = fil;
        if (histhue[fil] > maxhue) {
            maxhue = histhue[fil];
        }
    }
    av_frame_free(&s->frame_prev);
    s->frame_prev = av_frame_clone(in);
#define SET_META(key, fmt, val) do {                                \
    snprintf(metabuf, sizeof(metabuf), fmt, val);                   \
    av_dict_set(&out->metadata, "lavfi.signalstats." key, metabuf, 0);   \
} while (0)
    SET_META("YMIN",    "%d", miny);
    SET_META("YLOW",    "%d", lowy);
    SET_META("YAVG",    "%g", 1.0 * toty / s->fs);
    SET_META("YHIGH",   "%d", highy);
    SET_META("YMAX",    "%d", maxy);
    SET_META("UMIN",    "%d", minu);
    SET_META("ULOW",    "%d", lowu);
    SET_META("UAVG",    "%g", 1.0 * totu / s->cfs);
    SET_META("UHIGH",   "%d", highu);
    SET_META("UMAX",    "%d", maxu);
    SET_META("VMIN",    "%d", minv);
    SET_META("VLOW",    "%d", lowv);
    SET_META("VAVG",    "%g", 1.0 * totv / s->cfs);
    SET_META("VHIGH",   "%d", highv);
    SET_META("VMAX",    "%d", maxv);
    SET_META("SATMIN",  "%d", minsat);
    SET_META("SATLOW",  "%d", lowsat);
    SET_META("SATAVG",  "%g", 1.0 * totsat / s->cfs);
    SET_META("SATHIGH", "%d", highsat);
    SET_META("SATMAX",  "%d", maxsat);
    SET_META("HUEMED",  "%d", medhue);
    SET_META("HUEAVG",  "%g", 1.0 * tothue / s->cfs);
    SET_META("YDIF",    "%g", 1.0 * dify / s->fs);
    SET_META("UDIF",    "%g", 1.0 * difu / s->cfs);
    SET_META("VDIF",    "%g", 1.0 * difv / s->cfs);
    for (fil = 0; fil < FILT_NUMB; fil ++) {
        if (s->filters & 1<<fil) {
            char metaname[128];
            snprintf(metabuf,  sizeof(metabuf),  "%g", 1.0 * filtot[fil] / s->fs);
            snprintf(metaname, sizeof(metaname), "lavfi.signalstats.%s", filters_def[fil].name);
            av_dict_set(&out->metadata, metaname, metabuf, 0);
        }
    }
    if (in != out)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
