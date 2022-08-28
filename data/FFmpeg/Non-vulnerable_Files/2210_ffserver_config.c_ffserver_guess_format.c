static AVOutputFormat *ffserver_guess_format(const char *short_name,
                                             const char *filename,
                                             const char *mime_type)
{
    AVOutputFormat *fmt = av_guess_format(short_name, filename, mime_type);
    if (fmt) {
        AVOutputFormat *stream_fmt;
        char stream_format_name[64];
        snprintf(stream_format_name, sizeof(stream_format_name), "%s_stream",
                fmt->name);
        stream_fmt = av_guess_format(stream_format_name, NULL, NULL);
        if (stream_fmt)
            fmt = stream_fmt;
    }
    return fmt;
}
