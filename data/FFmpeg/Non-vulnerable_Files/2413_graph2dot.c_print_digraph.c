static void print_digraph(FILE *outfile, AVFilterGraph *graph)
{
    int i, j;
    fprintf(outfile, "digraph G {\n");
    fprintf(outfile, "node [shape=box]\n");
    fprintf(outfile, "rankdir=LR\n");
    for (i = 0; i < graph->nb_filters; i++) {
        char filter_ctx_label[128];
        const AVFilterContext *filter_ctx = graph->filters[i];
        snprintf(filter_ctx_label, sizeof(filter_ctx_label), "%s\\n(%s)",
                 filter_ctx->name,
                 filter_ctx->filter->name);
        for (j = 0; j < filter_ctx->nb_outputs; j++) {
            AVFilterLink *link = filter_ctx->outputs[j];
            if (link) {
                char dst_filter_ctx_label[128];
                const AVFilterContext *dst_filter_ctx = link->dst;
                snprintf(dst_filter_ctx_label, sizeof(dst_filter_ctx_label),
                         "%s\\n(%s)",
                         dst_filter_ctx->name,
                         dst_filter_ctx->filter->name);
                fprintf(outfile, "\"%s\" -> \"%s\" [ label= \"inpad:%s -> outpad:%s\\n",
                        filter_ctx_label, dst_filter_ctx_label,
                        avfilter_pad_get_name(link->srcpad, 0),
                        avfilter_pad_get_name(link->dstpad, 0));
                if (link->type == AVMEDIA_TYPE_VIDEO) {
                    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(link->format);
                    fprintf(outfile,
                            "fmt:%s w:%d h:%d tb:%d/%d",
                            desc->name,
                            link->w, link->h,
                            link->time_base.num, link->time_base.den);
                } else if (link->type == AVMEDIA_TYPE_AUDIO) {
                    char buf[255];
                    av_get_channel_layout_string(buf, sizeof(buf), -1,
                                                 link->channel_layout);
                    fprintf(outfile,
                            "fmt:%s sr:%d cl:%s tb:%d/%d",
                            av_get_sample_fmt_name(link->format),
                            link->sample_rate, buf,
                            link->time_base.num, link->time_base.den);
                }
                fprintf(outfile, "\" ];\n");
            }
        }
    }
    fprintf(outfile, "}\n");
}
