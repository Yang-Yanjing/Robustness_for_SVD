static void flush(AVFrame *out, AVFilterLink *outlink,
                  int *nb_samples_written, int *ret)
{
    if (*nb_samples_written) {
        out->nb_samples = *nb_samples_written / outlink->channels;
        *ret = ff_filter_frame(outlink, out);
        *nb_samples_written = 0;
    } else {
        av_frame_free(&out);
    }
}
