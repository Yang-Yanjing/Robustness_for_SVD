static int deinterlace_slice(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    W3FDIFContext *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *out = td->out;
    AVFrame *cur = td->cur;
    AVFrame *adj = td->adj;
    const int plane = td->plane;
    const int filter = s->filter;
    uint8_t *in_line, *in_lines_cur[5], *in_lines_adj[5];
    uint8_t *out_line, *out_pixel;
    int32_t *work_line, *work_pixel;
    uint8_t *cur_data = cur->data[plane];
    uint8_t *adj_data = adj->data[plane];
    uint8_t *dst_data = out->data[plane];
    const int linesize = s->linesize[plane];
    const int height   = s->planeheight[plane];
    const int cur_line_stride = cur->linesize[plane];
    const int adj_line_stride = adj->linesize[plane];
    const int dst_line_stride = out->linesize[plane];
    const int start = (height * jobnr) / nb_jobs;
    const int end = (height * (jobnr+1)) / nb_jobs;
    int i, j, y_in, y_out;
    
    y_out = start + (s->field == cur->top_field_first) - (start & 1);
    in_line  = cur_data + (y_out * cur_line_stride);
    out_line = dst_data + (y_out * dst_line_stride);
    while (y_out < end) {
        memcpy(out_line, in_line, linesize);
        y_out += 2;
        in_line  += cur_line_stride * 2;
        out_line += dst_line_stride * 2;
    }
    
    y_out = start + (s->field != cur->top_field_first) - (start & 1);
    out_line = dst_data + (y_out * dst_line_stride);
    while (y_out < end) {
        
        memset(s->work_line[jobnr], 0, sizeof(*s->work_line[jobnr]) * linesize);
        
        for (j = 0; j < n_coef_lf[filter]; j++) {
            y_in = (y_out + 1) + (j * 2) - n_coef_lf[filter];
            while (y_in < 0)
                y_in += 2;
            while (y_in >= height)
                y_in -= 2;
            in_lines_cur[j] = cur_data + (y_in * cur_line_stride);
        }
        work_line = s->work_line[jobnr];
        switch (n_coef_lf[filter]) {
        case 2:
            for (i = 0; i < linesize; i++) {
                *work_line   += *in_lines_cur[0]++ * coef_lf[filter][0];
                *work_line++ += *in_lines_cur[1]++ * coef_lf[filter][1];
            }
            break;
        case 4:
            for (i = 0; i < linesize; i++) {
                *work_line   += *in_lines_cur[0]++ * coef_lf[filter][0];
                *work_line   += *in_lines_cur[1]++ * coef_lf[filter][1];
                *work_line   += *in_lines_cur[2]++ * coef_lf[filter][2];
                *work_line++ += *in_lines_cur[3]++ * coef_lf[filter][3];
            }
        }
        
        for (j = 0; j < n_coef_hf[filter]; j++) {
            y_in = (y_out + 1) + (j * 2) - n_coef_hf[filter];
            while (y_in < 0)
                y_in += 2;
            while (y_in >= height)
                y_in -= 2;
            in_lines_cur[j] = cur_data + (y_in * cur_line_stride);
            in_lines_adj[j] = adj_data + (y_in * adj_line_stride);
        }
        work_line = s->work_line[jobnr];
        switch (n_coef_hf[filter]) {
        case 3:
            for (i = 0; i < linesize; i++) {
                *work_line   += *in_lines_cur[0]++ * coef_hf[filter][0];
                *work_line   += *in_lines_adj[0]++ * coef_hf[filter][0];
                *work_line   += *in_lines_cur[1]++ * coef_hf[filter][1];
                *work_line   += *in_lines_adj[1]++ * coef_hf[filter][1];
                *work_line   += *in_lines_cur[2]++ * coef_hf[filter][2];
                *work_line++ += *in_lines_adj[2]++ * coef_hf[filter][2];
            }
            break;
        case 5:
            for (i = 0; i < linesize; i++) {
                *work_line   += *in_lines_cur[0]++ * coef_hf[filter][0];
                *work_line   += *in_lines_adj[0]++ * coef_hf[filter][0];
                *work_line   += *in_lines_cur[1]++ * coef_hf[filter][1];
                *work_line   += *in_lines_adj[1]++ * coef_hf[filter][1];
                *work_line   += *in_lines_cur[2]++ * coef_hf[filter][2];
                *work_line   += *in_lines_adj[2]++ * coef_hf[filter][2];
                *work_line   += *in_lines_cur[3]++ * coef_hf[filter][3];
                *work_line   += *in_lines_adj[3]++ * coef_hf[filter][3];
                *work_line   += *in_lines_cur[4]++ * coef_hf[filter][4];
                *work_line++ += *in_lines_adj[4]++ * coef_hf[filter][4];
            }
        }
        
        work_pixel = s->work_line[jobnr];
        out_pixel = out_line;
        for (j = 0; j < linesize; j++, out_pixel++, work_pixel++)
             *out_pixel = av_clip(*work_pixel, 0, 255 * 256 * 256) >> 16;
        
        y_out += 2;
        out_line += dst_line_stride * 2;
    }
    return 0;
}
