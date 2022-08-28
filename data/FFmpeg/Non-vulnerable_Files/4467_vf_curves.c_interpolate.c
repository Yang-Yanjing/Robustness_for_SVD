static int interpolate(AVFilterContext *ctx, uint8_t *y, const struct keypoint *points)
{
    int i, ret = 0;
    const struct keypoint *point;
    double xprev = 0;
    int n = get_nb_points(points); 
    double (*matrix)[3] = av_calloc(n, sizeof(*matrix));
    double *h = av_malloc((n - 1) * sizeof(*h));
    double *r = av_calloc(n, sizeof(*r));
    if (!matrix || !h || !r) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    
    i = -1;
    for (point = points; point; point = point->next) {
        if (i != -1)
            h[i] = point->x - xprev;
        xprev = point->x;
        i++;
    }
    
    point = points;
    for (i = 1; i < n - 1; i++) {
        double yp = point->y,
               yc = point->next->y,
               yn = point->next->next->y;
        r[i] = 6 * ((yn-yc)/h[i] - (yc-yp)/h[i-1]);
        point = point->next;
    }
#define BD 0 
#define MD 1 
#define AD 2 
    
    matrix[0][MD] = matrix[n - 1][MD] = 1;
    for (i = 1; i < n - 1; i++) {
        matrix[i][BD] = h[i-1];
        matrix[i][MD] = 2 * (h[i-1] + h[i]);
        matrix[i][AD] = h[i];
    }
    
    for (i = 1; i < n; i++) {
        double den = matrix[i][MD] - matrix[i][BD] * matrix[i-1][AD];
        double k = den ? 1./den : 1.;
        matrix[i][AD] *= k;
        r[i] = (r[i] - matrix[i][BD] * r[i - 1]) * k;
    }
    for (i = n - 2; i >= 0; i--)
        r[i] = r[i] - matrix[i][AD] * r[i + 1];
    
    i = 0;
    point = points;
    av_assert0(point->next); 
    while (point->next) {
        double yc = point->y;
        double yn = point->next->y;
        double a = yc;
        double b = (yn-yc)/h[i] - h[i]*r[i]/2. - h[i]*(r[i+1]-r[i])/6.;
        double c = r[i] / 2.;
        double d = (r[i+1] - r[i]) / (6.*h[i]);
        int x;
        int x_start = point->x       * 255;
        int x_end   = point->next->x * 255;
        av_assert0(x_start >= 0 && x_start <= 255 &&
                   x_end   >= 0 && x_end   <= 255);
        for (x = x_start; x <= x_end; x++) {
            double xx = (x - x_start) * 1/255.;
            double yy = a + b*xx + c*xx*xx + d*xx*xx*xx;
            y[x] = av_clipf(yy, 0, 1) * 255;
            av_log(ctx, AV_LOG_DEBUG, "f(%f)=%f -> y[%d]=%d\n", xx, yy, x, y[x]);
        }
        point = point->next;
        i++;
    }
end:
    av_free(matrix);
    av_free(h);
    av_free(r);
    return ret;
}
