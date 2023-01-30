static int scale_slice(AVFilterLink *link, AVFrame *out_buf, AVFrame *cur_pic, struct SwsContext *sws, int y, int h, int mul, int field)
{
    ScaleContext *scale = link->dst->priv;
    const uint8_t *in[4];
    uint8_t *out[4];
    int in_stride[4],out_stride[4];
    int i;
    for(i=0; i<4; i++){
        int vsub= ((i+1)&2) ? scale->vsub : 0;
         in_stride[i] = cur_pic->linesize[i] * mul;
        out_stride[i] = out_buf->linesize[i] * mul;
         in[i] = cur_pic->data[i] + ((y>>vsub)+field) * cur_pic->linesize[i];
        out[i] = out_buf->data[i] +            field  * out_buf->linesize[i];
    }
    if(scale->input_is_pal)
         in[1] = cur_pic->data[1];
    if(scale->output_is_pal)
        out[1] = out_buf->data[1];
    return sws_scale(sws, in, in_stride, y/mul, h,
                         out,out_stride);
}
