static AVFrame *get_best_frame(AVFilterContext *ctx)
{
    AVFrame *picref;
    ThumbContext *thumb = ctx->priv;
    int i, j, best_frame_idx = 0;
    int nb_frames = thumb->n;
    double avg_hist[HIST_SIZE] = {0}, sq_err, min_sq_err = -1;
    
    for (j = 0; j < FF_ARRAY_ELEMS(avg_hist); j++) {
        for (i = 0; i < nb_frames; i++)
            avg_hist[j] += (double)thumb->frames[i].histogram[j];
        avg_hist[j] /= nb_frames;
    }
    
    for (i = 0; i < nb_frames; i++) {
        sq_err = frame_sum_square_err(thumb->frames[i].histogram, avg_hist);
        if (i == 0 || sq_err < min_sq_err)
            best_frame_idx = i, min_sq_err = sq_err;
    }
    
    for (i = 0; i < nb_frames; i++) {
        memset(thumb->frames[i].histogram, 0, sizeof(thumb->frames[i].histogram));
        if (i != best_frame_idx)
            av_frame_free(&thumb->frames[i].buf);
    }
    thumb->n = 0;
    
    picref = thumb->frames[best_frame_idx].buf;
    av_log(ctx, AV_LOG_INFO, "frame id #%d (pts_time=%f) selected "
           "from a set of %d images\n", best_frame_idx,
           picref->pts * av_q2d(thumb->tb), nb_frames);
    thumb->frames[best_frame_idx].buf = NULL;
    return picref;
}
