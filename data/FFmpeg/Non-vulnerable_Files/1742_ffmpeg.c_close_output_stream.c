static void close_output_stream(OutputStream *ost)
{
    OutputFile *of = output_files[ost->file_index];
    ost->finished |= ENCODER_FINISHED;
    if (of->shortest) {
        int64_t end = av_rescale_q(ost->sync_opts - ost->first_pts, ost->enc_ctx->time_base, AV_TIME_BASE_Q);
        of->recording_time = FFMIN(of->recording_time, end);
    }
}
