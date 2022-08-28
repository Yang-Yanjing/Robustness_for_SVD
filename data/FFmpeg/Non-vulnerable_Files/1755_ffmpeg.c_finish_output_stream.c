static int init_output_stream(OutputStream *ost, char *error, int error_len);
static void finish_output_stream(OutputStream *ost)
{
    OutputFile *of = output_files[ost->file_index];
    int i;
    ost->finished = ENCODER_FINISHED | MUXER_FINISHED;
    if (of->shortest) {
        for (i = 0; i < of->ctx->nb_streams; i++)
            output_streams[of->ost_index + i]->finished = ENCODER_FINISHED | MUXER_FINISHED;
    }
}
