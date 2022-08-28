static int deshake_transform_c(AVFilterContext *ctx,
                                    int width, int height, int cw, int ch,
                                    const float *matrix_y, const float *matrix_uv,
                                    enum InterpolateMethod interpolate,
                                    enum FillMethod fill, AVFrame *in, AVFrame *out)
{
    int i = 0, ret = 0;
    const float *matrixs[3];
    int plane_w[3], plane_h[3];
    matrixs[0] = matrix_y;
    matrixs[1] =  matrixs[2] = matrix_uv;
    plane_w[0] = width;
    plane_w[1] = plane_w[2] = cw;
    plane_h[0] = height;
    plane_h[1] = plane_h[2] = ch;
    for (i = 0; i < 3; i++) {
        
        ret = avfilter_transform(in->data[i], out->data[i], in->linesize[i], out->linesize[i],
                                 plane_w[i], plane_h[i], matrixs[i], interpolate, fill);
        if (ret < 0)
            return ret;
    }
    return ret;
}
