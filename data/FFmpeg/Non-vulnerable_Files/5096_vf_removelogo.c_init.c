static av_cold int init(AVFilterContext *ctx)
{
    RemovelogoContext *s = ctx->priv;
    int ***mask;
    int ret = 0;
    int a, b, c, w, h;
    int full_max_mask_size, half_max_mask_size;
    if (!s->filename) {
        av_log(ctx, AV_LOG_ERROR, "The bitmap file name is mandatory\n");
        return AVERROR(EINVAL);
    }
    
    if ((ret = load_mask(&s->full_mask_data, &w, &h, s->filename, ctx)) < 0)
        return ret;
    s->mask_w = w;
    s->mask_h = h;
    convert_mask_to_strength_mask(s->full_mask_data, w, w, h,
                                  16, &full_max_mask_size);
    
    if (!(s->half_mask_data = av_mallocz(w/2 * h/2)))
        return AVERROR(ENOMEM);
    generate_half_size_image(s->full_mask_data, w,
                             s->half_mask_data, w/2,
                             w, h, &half_max_mask_size);
    s->max_mask_size = FFMAX(full_max_mask_size, half_max_mask_size);
    



    mask = (int ***)av_malloc_array(s->max_mask_size + 1, sizeof(int **));
    if (!mask)
        return AVERROR(ENOMEM);
    for (a = 0; a <= s->max_mask_size; a++) {
        mask[a] = (int **)av_malloc_array((a * 2) + 1, sizeof(int *));
        if (!mask[a]) {
            av_free(mask);
            return AVERROR(ENOMEM);
        }
        for (b = -a; b <= a; b++) {
            mask[a][b + a] = (int *)av_malloc_array((a * 2) + 1, sizeof(int));
            if (!mask[a][b + a]) {
                av_free(mask);
                return AVERROR(ENOMEM);
            }
            for (c = -a; c <= a; c++) {
                if ((b * b) + (c * c) <= (a * a)) 
                    mask[a][b + a][c + a] = 1;
                else
                    mask[a][b + a][c + a] = 0;
            }
        }
    }
    s->mask = mask;
    

    ff_calculate_bounding_box(&s->full_mask_bbox, s->full_mask_data, w, w, h, 0);
    ff_calculate_bounding_box(&s->half_mask_bbox, s->half_mask_data, w/2, w/2, h/2, 0);
#define SHOW_LOGO_INFO(mask_type)                                       \
    av_log(ctx, AV_LOG_VERBOSE, #mask_type " x1:%d x2:%d y1:%d y2:%d max_mask_size:%d\n", \
           s->mask_type##_mask_bbox.x1, s->mask_type##_mask_bbox.x2, \
           s->mask_type##_mask_bbox.y1, s->mask_type##_mask_bbox.y2, \
           mask_type##_max_mask_size);
    SHOW_LOGO_INFO(full);
    SHOW_LOGO_INFO(half);
    return 0;
}
