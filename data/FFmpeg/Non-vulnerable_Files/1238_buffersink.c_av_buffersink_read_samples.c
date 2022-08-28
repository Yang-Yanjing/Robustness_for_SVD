int attribute_align_arg av_buffersink_read_samples(AVFilterContext *ctx, AVFilterBufferRef **buf,
                                                   int nb_samples)
{
    return compat_read(ctx, buf, nb_samples, 0);
}
