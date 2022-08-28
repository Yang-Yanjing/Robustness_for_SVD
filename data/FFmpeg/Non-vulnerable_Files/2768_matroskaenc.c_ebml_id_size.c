static int ebml_id_size(unsigned int id)
{
    return (av_log2(id + 1) - 1) / 7 + 1;
}
