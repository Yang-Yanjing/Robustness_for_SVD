static void check_decode_result(InputStream *ist, int *got_output, int ret)
{
    if (*got_output || ret<0)
        decode_error_stat[ret<0] ++;
    if (ret < 0 && exit_on_error)
        exit_program(1);
    if (exit_on_error && *got_output && ist) {
        if (av_frame_get_decode_error_flags(ist->decoded_frame) || (ist->decoded_frame->flags & AV_FRAME_FLAG_CORRUPT)) {
            av_log(NULL, AV_LOG_FATAL, "%s: corrupt decoded frame in stream %d\n", input_files[ist->file_index]->ctx->filename, ist->st->index);
            exit_program(1);
        }
    }
}
