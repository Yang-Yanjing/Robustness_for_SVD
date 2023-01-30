static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    uint32_t plane_checksum[4] = {0}, checksum = 0;
    int64_t sum[4] = {0}, sum2[4] = {0};
    int32_t pixelcount[4] = {0};
    int i, plane, vsub = desc->log2_chroma_h;
    for (plane = 0; plane < 4 && frame->data[plane] && frame->linesize[plane]; plane++) {
        uint8_t *data = frame->data[plane];
        int h = plane == 1 || plane == 2 ? FF_CEIL_RSHIFT(inlink->h, vsub) : inlink->h;
        int linesize = av_image_get_linesize(frame->format, frame->width, plane);
        if (linesize < 0)
            return linesize;
        for (i = 0; i < h; i++) {
            plane_checksum[plane] = av_adler32_update(plane_checksum[plane], data, linesize);
            checksum = av_adler32_update(checksum, data, linesize);
            update_sample_stats(data, linesize, sum+plane, sum2+plane);
            pixelcount[plane] += linesize;
            data += frame->linesize[plane];
        }
    }
    av_log(ctx, AV_LOG_INFO,
           "n:%4"PRId64" pts:%7s pts_time:%-7s pos:%9"PRId64" "
           "fmt:%s sar:%d/%d s:%dx%d i:%c iskey:%d type:%c "
           "checksum:%08"PRIX32" plane_checksum:[%08"PRIX32,
           inlink->frame_count,
           av_ts2str(frame->pts), av_ts2timestr(frame->pts, &inlink->time_base), av_frame_get_pkt_pos(frame),
           desc->name,
           frame->sample_aspect_ratio.num, frame->sample_aspect_ratio.den,
           frame->width, frame->height,
           !frame->interlaced_frame ? 'P' :         
           frame->top_field_first   ? 'T' : 'B',    
           frame->key_frame,
           av_get_picture_type_char(frame->pict_type),
           checksum, plane_checksum[0]);
    for (plane = 1; plane < 4 && frame->data[plane] && frame->linesize[plane]; plane++)
        av_log(ctx, AV_LOG_INFO, " %08"PRIX32, plane_checksum[plane]);
    av_log(ctx, AV_LOG_INFO, "] mean:[");
    for (plane = 0; plane < 4 && frame->data[plane] && frame->linesize[plane]; plane++)
        av_log(ctx, AV_LOG_INFO, "%"PRId64" ", (sum[plane] + pixelcount[plane]/2) / pixelcount[plane]);
    av_log(ctx, AV_LOG_INFO, "\b] stdev:[");
    for (plane = 0; plane < 4 && frame->data[plane] && frame->linesize[plane]; plane++)
        av_log(ctx, AV_LOG_INFO, "%3.1f ",
               sqrt((sum2[plane] - sum[plane]*(double)sum[plane]/pixelcount[plane])/pixelcount[plane]));
    av_log(ctx, AV_LOG_INFO, "\b]\n");
    for (i = 0; i < frame->nb_side_data; i++) {
        AVFrameSideData *sd = frame->side_data[i];
        av_log(ctx, AV_LOG_INFO, "  side data - ");
        switch (sd->type) {
        case AV_FRAME_DATA_PANSCAN:
            av_log(ctx, AV_LOG_INFO, "pan/scan");
            break;
        case AV_FRAME_DATA_A53_CC:
            av_log(ctx, AV_LOG_INFO, "A/53 closed captions (%d bytes)", sd->size);
            break;
        case AV_FRAME_DATA_STEREO3D:
            dump_stereo3d(ctx, sd);
            break;
        case AV_FRAME_DATA_DISPLAYMATRIX:
            av_log(ctx, AV_LOG_INFO, "displaymatrix: rotation of %.2f degrees",
                   av_display_rotation_get((int32_t *)sd->data));
            break;
        case AV_FRAME_DATA_AFD:
            av_log(ctx, AV_LOG_INFO, "afd: value of %"PRIu8, sd->data[0]);
            break;
        default:
            av_log(ctx, AV_LOG_WARNING, "unknown side data type %d (%d bytes)",
                   sd->type, sd->size);
            break;
        }
        av_log(ctx, AV_LOG_INFO, "\n");
    }
    return ff_filter_frame(inlink->dst->outputs[0], frame);
}
