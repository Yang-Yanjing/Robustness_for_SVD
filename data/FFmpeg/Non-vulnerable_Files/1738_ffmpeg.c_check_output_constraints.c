static int check_output_constraints(InputStream *ist, OutputStream *ost)
{
    OutputFile *of = output_files[ost->file_index];
    int ist_index  = input_files[ist->file_index]->ist_index + ist->st->index;
    if (ost->source_index != ist_index)
        return 0;
    if (ost->finished)
        return 0;
    if (of->start_time != AV_NOPTS_VALUE && ist->pts < of->start_time)
        return 0;
    return 1;
}
