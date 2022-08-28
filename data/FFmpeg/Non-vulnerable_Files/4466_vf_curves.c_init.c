static av_cold int init(AVFilterContext *ctx)
{
    int i, j, ret;
    CurvesContext *curves = ctx->priv;
    struct keypoint *comp_points[NB_COMP + 1] = {0};
    char **pts = curves->comp_points_str;
    const char *allp = curves->comp_points_str_all;
    
    
    if (allp) {
        for (i = 0; i < NB_COMP; i++) {
            if (!pts[i])
                pts[i] = av_strdup(allp);
            if (!pts[i])
                return AVERROR(ENOMEM);
        }
    }
    if (curves->psfile) {
        ret = parse_psfile(ctx, curves->psfile);
        if (ret < 0)
            return ret;
    }
    if (curves->preset != PRESET_NONE) {
#define SET_COMP_IF_NOT_SET(n, name) do {                           \
    if (!pts[n] && curves_presets[curves->preset].name) {           \
        pts[n] = av_strdup(curves_presets[curves->preset].name);    \
        if (!pts[n])                                                \
            return AVERROR(ENOMEM);                                 \
    }                                                               \
} while (0)
        SET_COMP_IF_NOT_SET(0, r);
        SET_COMP_IF_NOT_SET(1, g);
        SET_COMP_IF_NOT_SET(2, b);
        SET_COMP_IF_NOT_SET(3, master);
    }
    for (i = 0; i < NB_COMP + 1; i++) {
        ret = parse_points_str(ctx, comp_points + i, curves->comp_points_str[i]);
        if (ret < 0)
            return ret;
        ret = interpolate(ctx, curves->graph[i], comp_points[i]);
        if (ret < 0)
            return ret;
    }
    if (pts[NB_COMP]) {
        for (i = 0; i < NB_COMP; i++)
            for (j = 0; j < 256; j++)
                curves->graph[i][j] = curves->graph[NB_COMP][curves->graph[i][j]];
    }
    if (av_log_get_level() >= AV_LOG_VERBOSE) {
        for (i = 0; i < NB_COMP; i++) {
            struct keypoint *point = comp_points[i];
            av_log(ctx, AV_LOG_VERBOSE, "#%d points:", i);
            while (point) {
                av_log(ctx, AV_LOG_VERBOSE, " (%f;%f)", point->x, point->y);
                point = point->next;
            }
            av_log(ctx, AV_LOG_VERBOSE, "\n");
            av_log(ctx, AV_LOG_VERBOSE, "#%d values:", i);
            for (j = 0; j < 256; j++)
                av_log(ctx, AV_LOG_VERBOSE, " %02X", curves->graph[i][j]);
            av_log(ctx, AV_LOG_VERBOSE, "\n");
        }
    }
    for (i = 0; i < NB_COMP + 1; i++) {
        struct keypoint *point = comp_points[i];
        while (point) {
            struct keypoint *next = point->next;
            av_free(point);
            point = next;
        }
    }
    return 0;
}
