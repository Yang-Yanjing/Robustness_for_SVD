int8_t *av_frame_get_qp_table(AVFrame *f, int *stride, int *type)
{
    *stride = f->qstride;
    *type   = f->qscale_type;
    if (!f->qp_table_buf)
        return NULL;
    return f->qp_table_buf->data;
}
