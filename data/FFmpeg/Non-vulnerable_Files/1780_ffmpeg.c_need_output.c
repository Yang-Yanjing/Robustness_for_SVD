static int need_output(void)
{
    int i;
    for (i = 0; i < nb_output_streams; i++) {
        OutputStream *ost    = output_streams[i];
        OutputFile *of       = output_files[ost->file_index];
        AVFormatContext *os  = output_files[ost->file_index]->ctx;
        if (ost->finished ||
            (os->pb && avio_tell(os->pb) >= of->limit_filesize))
            continue;
        if (ost->frame_number >= ost->max_frames) {
            int j;
            for (j = 0; j < of->ctx->nb_streams; j++)
                close_output_stream(output_streams[of->ost_index + j]);
            continue;
        }
        return 1;
    }
    return 0;
}
