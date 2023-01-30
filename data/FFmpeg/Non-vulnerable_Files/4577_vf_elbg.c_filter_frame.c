static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    ELBGContext *elbg = inlink->dst->priv;
    int i, j, k;
    uint8_t *p, *p0;
    const uint8_t r_idx  = elbg->rgba_map[R];
    const uint8_t g_idx  = elbg->rgba_map[G];
    const uint8_t b_idx  = elbg->rgba_map[B];
    
    p0 = frame->data[0];
    k = 0;
    for (i = 0; i < inlink->h; i++) {
        p = p0;
        for (j = 0; j < inlink->w; j++) {
            elbg->codeword[k++] = p[r_idx];
            elbg->codeword[k++] = p[g_idx];
            elbg->codeword[k++] = p[b_idx];
            p += elbg->pix_desc->nb_components;
        }
        p0 += frame->linesize[0];
    }
    
    avpriv_init_elbg(elbg->codeword, NB_COMPONENTS, elbg->codeword_length,
                     elbg->codebook, elbg->codebook_length, elbg->max_steps_nb,
                     elbg->codeword_closest_codebook_idxs, &elbg->lfg);
    avpriv_do_elbg(elbg->codeword, NB_COMPONENTS, elbg->codeword_length,
                   elbg->codebook, elbg->codebook_length, elbg->max_steps_nb,
                   elbg->codeword_closest_codebook_idxs, &elbg->lfg);
    if (elbg->pal8) {
        AVFilterLink *outlink = inlink->dst->outputs[0];
        AVFrame *out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        uint32_t *pal;
        if (!out)
            return AVERROR(ENOMEM);
        out->pts = frame->pts;
        av_frame_free(&frame);
        pal = (uint32_t *)out->data[1];
        p0 = (uint8_t *)out->data[0];
        for (i = 0; i < elbg->codebook_length; i++) {
            pal[i] = (elbg->codebook[i*3  ] << 16) |
                     (elbg->codebook[i*3+1] <<  8) |
                      elbg->codebook[i*3+2];
        }
        k = 0;
        for (i = 0; i < inlink->h; i++) {
            p = p0;
            for (j = 0; j < inlink->w; j++, p++) {
                p[0] = elbg->codeword_closest_codebook_idxs[k++];
            }
            p0 += out->linesize[0];
        }
        return ff_filter_frame(outlink, out);
    }
    
    p0 = frame->data[0];
    k = 0;
    for (i = 0; i < inlink->h; i++) {
        p = p0;
        for (j = 0; j < inlink->w; j++) {
            int cb_idx = NB_COMPONENTS * elbg->codeword_closest_codebook_idxs[k++];
            p[r_idx] = elbg->codebook[cb_idx];
            p[g_idx] = elbg->codebook[cb_idx+1];
            p[b_idx] = elbg->codebook[cb_idx+2];
            p += elbg->pix_desc->nb_components;
        }
        p0 += frame->linesize[0];
    }
    return ff_filter_frame(inlink->dst->outputs[0], frame);
}
