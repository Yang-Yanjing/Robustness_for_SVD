int av_frame_set_qp_table(AVFrame *f, AVBufferRef *buf, int stride, int qp_type)
{
    av_buffer_unref(&f->qp_table_buf);
    f->qp_table_buf = buf;
    f->qscale_table = buf->data;
    f->qstride      = stride;
    f->qscale_type  = qp_type;
    return 0;
}
