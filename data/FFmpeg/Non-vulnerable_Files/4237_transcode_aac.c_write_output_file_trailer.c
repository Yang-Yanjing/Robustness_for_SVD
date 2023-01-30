static int write_output_file_trailer(AVFormatContext *output_format_context)
{
    int error;
    if ((error = av_write_trailer(output_format_context)) < 0) {
        fprintf(stderr, "Could not write output file trailer (error '%s')\n",
                get_error_text(error));
        return error;
    }
    return 0;
}
