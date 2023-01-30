static int copy_video_props(AVFilterBufferRefVideoProps *dst, AVFilterBufferRefVideoProps *src) {
    *dst = *src;
    if (src->qp_table) {
        int qsize = src->qp_table_size;
        dst->qp_table = av_malloc(qsize);
        if (!dst->qp_table) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate qp_table\n");
            dst->qp_table_size = 0;
            return AVERROR(ENOMEM);
        }
        memcpy(dst->qp_table, src->qp_table, qsize);
    }
    return 0;
}
