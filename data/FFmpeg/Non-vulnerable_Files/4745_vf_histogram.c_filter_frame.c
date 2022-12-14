static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    HistogramContext *h   = inlink->dst->priv;
    AVFilterContext *ctx  = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    uint8_t *dst;
    int i, j, k, l, m;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    out->pts = in->pts;
    for (k = 0; k < 4 && out->data[k]; k++) {
        const int is_chroma = (k == 1 || k == 2);
        const int dst_h = FF_CEIL_RSHIFT(outlink->h, (is_chroma ? h->odesc->log2_chroma_h : 0));
        const int dst_w = FF_CEIL_RSHIFT(outlink->w, (is_chroma ? h->odesc->log2_chroma_w : 0));
        if (h->histogram_size <= 256) {
            for (i = 0; i < dst_h ; i++)
                memset(out->data[h->odesc->comp[k].plane] +
                       i * out->linesize[h->odesc->comp[k].plane],
                       h->bg_color[k], dst_w);
        } else {
            const int mult = h->mult;
            for (i = 0; i < dst_h ; i++)
                for (j = 0; j < dst_w; j++)
                    AV_WN16(out->data[h->odesc->comp[k].plane] +
                        i * out->linesize[h->odesc->comp[k].plane] + j * 2,
                        h->bg_color[k] * mult);
        }
    }
    switch (h->mode) {
    case MODE_LEVELS:
        for (m = 0, k = 0; k < h->ncomp; k++) {
            const int p = h->desc->comp[k].plane;
            const int height = h->planeheight[p];
            const int width = h->planewidth[p];
            double max_hval_log;
            unsigned max_hval = 0;
            int start;
            if (!((1 << k) & h->components))
                continue;
            start = m++ * (h->level_height + h->scale_height) * h->display_mode;
            if (h->histogram_size <= 256) {
                for (i = 0; i < height; i++) {
                    const uint8_t *src = in->data[p] + i * in->linesize[p];
                    for (j = 0; j < width; j++)
                        h->histogram[src[j]]++;
                }
            } else {
                for (i = 0; i < height; i++) {
                    const uint16_t *src = (const uint16_t *)(in->data[p] + i * in->linesize[p]);
                    for (j = 0; j < width; j++)
                        h->histogram[src[j]]++;
                }
            }
            for (i = 0; i < h->histogram_size; i++)
                max_hval = FFMAX(max_hval, h->histogram[i]);
            max_hval_log = log2(max_hval + 1);
            for (i = 0; i < outlink->w; i++) {
                int col_height;
                if (h->levels_mode)
                    col_height = round(h->level_height * (1. - (log2(h->histogram[i] + 1) / max_hval_log)));
                else
                    col_height = h->level_height - (h->histogram[i] * (int64_t)h->level_height + max_hval - 1) / max_hval;
                if (h->histogram_size <= 256) {
                    for (j = h->level_height - 1; j >= col_height; j--) {
                        if (h->display_mode) {
                            for (l = 0; l < h->ncomp; l++)
                                out->data[l][(j + start) * out->linesize[l] + i] = h->fg_color[l];
                        } else {
                            out->data[p][(j + start) * out->linesize[p] + i] = 255;
                        }
                    }
                    for (j = h->level_height + h->scale_height - 1; j >= h->level_height; j--)
                        out->data[p][(j + start) * out->linesize[p] + i] = i;
                } else {
                    const int mult = h->mult;
                    for (j = h->level_height - 1; j >= col_height; j--) {
                        if (h->display_mode) {
                            for (l = 0; l < h->ncomp; l++)
                                AV_WN16(out->data[l] + (j + start) * out->linesize[l] + i * 2, h->fg_color[l] * mult);
                        } else {
                            AV_WN16(out->data[p] + (j + start) * out->linesize[p] + i * 2, 255 * mult);
                        }
                    }
                    for (j = h->level_height + h->scale_height - 1; j >= h->level_height; j--)
                        AV_WN16(out->data[p] + (j + start) * out->linesize[p] + i * 2, i);
                }
            }
            memset(h->histogram, 0, h->histogram_size * sizeof(unsigned));
        }
        break;
    case MODE_WAVEFORM:
        for (k = 0; k < h->ncomp; k++) {
            const int offset = k * 256 * h->display_mode;
            gen_waveform(h, in, out, k, h->step, offset, h->waveform_mode);
        }
        break;
    case MODE_COLOR:
        for (i = 0; i < inlink->h; i++) {
            const int iw1 = i * in->linesize[1];
            const int iw2 = i * in->linesize[2];
            for (j = 0; j < inlink->w; j++) {
                const int pos = in->data[1][iw1 + j] * out->linesize[0] + in->data[2][iw2 + j];
                if (out->data[0][pos] < 255)
                    out->data[0][pos]++;
            }
        }
        for (i = 0; i < 256; i++) {
            dst = out->data[0] + i * out->linesize[0];
            for (j = 0; j < 256; j++) {
                if (!dst[j]) {
                    out->data[1][i * out->linesize[0] + j] = i;
                    out->data[2][i * out->linesize[0] + j] = j;
                }
            }
        }
        break;
    case MODE_COLOR2:
        for (i = 0; i < inlink->h; i++) {
            const int iw1 = i * in->linesize[1];
            const int iw2 = i * in->linesize[2];
            for (j = 0; j < inlink->w; j++) {
                const int u = in->data[1][iw1 + j];
                const int v = in->data[2][iw2 + j];
                const int pos = u * out->linesize[0] + v;
                if (!out->data[0][pos])
                    out->data[0][pos] = FFABS(128 - u) + FFABS(128 - v);
                out->data[1][pos] = u;
                out->data[2][pos] = v;
            }
        }
        break;
    default:
        av_assert0(0);
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
