static int parse_points_str(AVFilterContext *ctx, struct keypoint **points, const char *s)
{
    char *p = (char *)s; 
    struct keypoint *last = NULL;
    
    while (p && *p) {
        struct keypoint *point = make_point(0, 0, NULL);
        if (!point)
            return AVERROR(ENOMEM);
        point->x = av_strtod(p, &p); if (p && *p) p++;
        point->y = av_strtod(p, &p); if (p && *p) p++;
        if (point->x < 0 || point->x > 1 || point->y < 0 || point->y > 1) {
            av_log(ctx, AV_LOG_ERROR, "Invalid key point coordinates (%f;%f), "
                   "x and y must be in the [0;1] range.\n", point->x, point->y);
            return AVERROR(EINVAL);
        }
        if (!*points)
            *points = point;
        if (last) {
            if ((int)(last->x * 255) >= (int)(point->x * 255)) {
                av_log(ctx, AV_LOG_ERROR, "Key point coordinates (%f;%f) "
                       "and (%f;%f) are too close from each other or not "
                       "strictly increasing on the x-axis\n",
                       last->x, last->y, point->x, point->y);
                return AVERROR(EINVAL);
            }
            last->next = point;
        }
        last = point;
    }
    
    if (!*points) {
        last = make_point(0, 0, NULL);
        if (!last)
            return AVERROR(ENOMEM);
        last->x = last->y = 0;
        *points = last;
    } else if ((*points)->x != 0.) {
        struct keypoint *newfirst = make_point(0, 0, *points);
        if (!newfirst)
            return AVERROR(ENOMEM);
        *points = newfirst;
    }
    av_assert0(last);
    
    if (last->x != 1.) {
        struct keypoint *point = make_point(1, 1, NULL);
        if (!point)
            return AVERROR(ENOMEM);
        last->next = point;
    }
    return 0;
}
