static int filter_frame(AVFilterLink *link, AVFrame *in)
{
    DeshakeContext *deshake = link->dst->priv;
    AVFilterLink *outlink = link->dst->outputs[0];
    AVFrame *out;
    Transform t = {{0},0}, orig = {{0},0};
    float matrix_y[9], matrix_uv[9];
    float alpha = 2.0 / deshake->refcount;
    char tmp[256];
    int ret = 0;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    if (CONFIG_OPENCL && deshake->opencl) {
        ret = ff_opencl_deshake_process_inout_buf(link->dst,in, out);
        if (ret < 0)
            return ret;
    }
    if (deshake->cx < 0 || deshake->cy < 0 || deshake->cw < 0 || deshake->ch < 0) {
        
        find_motion(deshake, (deshake->ref == NULL) ? in->data[0] : deshake->ref->data[0], in->data[0], link->w, link->h, in->linesize[0], &t);
    } else {
        uint8_t *src1 = (deshake->ref == NULL) ? in->data[0] : deshake->ref->data[0];
        uint8_t *src2 = in->data[0];
        deshake->cx = FFMIN(deshake->cx, link->w);
        deshake->cy = FFMIN(deshake->cy, link->h);
        if ((unsigned)deshake->cx + (unsigned)deshake->cw > link->w) deshake->cw = link->w - deshake->cx;
        if ((unsigned)deshake->cy + (unsigned)deshake->ch > link->h) deshake->ch = link->h - deshake->cy;
        
        deshake->cw &= ~15;
        src1 += deshake->cy * in->linesize[0] + deshake->cx;
        src2 += deshake->cy * in->linesize[0] + deshake->cx;
        find_motion(deshake, src1, src2, deshake->cw, deshake->ch, in->linesize[0], &t);
    }
    
    orig.vec.x = t.vec.x;
    orig.vec.y = t.vec.y;
    orig.angle = t.angle;
    orig.zoom = t.zoom;
    
    deshake->avg.vec.x = alpha * t.vec.x + (1.0 - alpha) * deshake->avg.vec.x;
    deshake->avg.vec.y = alpha * t.vec.y + (1.0 - alpha) * deshake->avg.vec.y;
    deshake->avg.angle = alpha * t.angle + (1.0 - alpha) * deshake->avg.angle;
    deshake->avg.zoom = alpha * t.zoom + (1.0 - alpha) * deshake->avg.zoom;
    
    
    t.vec.x -= deshake->avg.vec.x;
    t.vec.y -= deshake->avg.vec.y;
    t.angle -= deshake->avg.angle;
    t.zoom -= deshake->avg.zoom;
    
    t.vec.x *= -1;
    t.vec.y *= -1;
    t.angle *= -1;
    
    if (deshake->fp) {
        snprintf(tmp, 256, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", orig.vec.x, deshake->avg.vec.x, t.vec.x, orig.vec.y, deshake->avg.vec.y, t.vec.y, orig.angle, deshake->avg.angle, t.angle, orig.zoom, deshake->avg.zoom, t.zoom);
        fwrite(tmp, sizeof(char), strlen(tmp), deshake->fp);
    }
    
    
    t.vec.x += deshake->last.vec.x;
    t.vec.y += deshake->last.vec.y;
    t.angle += deshake->last.angle;
    t.zoom += deshake->last.zoom;
    
    t.vec.x *= 0.9;
    t.vec.y *= 0.9;
    t.angle *= 0.9;
    
    deshake->last.vec.x = t.vec.x;
    deshake->last.vec.y = t.vec.y;
    deshake->last.angle = t.angle;
    deshake->last.zoom = t.zoom;
    
    avfilter_get_matrix(t.vec.x, t.vec.y, t.angle, 1.0 + t.zoom / 100.0, matrix_y);
    
    avfilter_get_matrix(t.vec.x / (link->w / CHROMA_WIDTH(link)), t.vec.y / (link->h / CHROMA_HEIGHT(link)), t.angle, 1.0 + t.zoom / 100.0, matrix_uv);
    
    ret = deshake->transform(link->dst, link->w, link->h, CHROMA_WIDTH(link), CHROMA_HEIGHT(link),
                             matrix_y, matrix_uv, INTERPOLATE_BILINEAR, deshake->edge, in, out);
    
    av_frame_free(&deshake->ref);
    if (ret < 0)
        return ret;
    
    
    deshake->ref = in;
    return ff_filter_frame(outlink, out);
}
