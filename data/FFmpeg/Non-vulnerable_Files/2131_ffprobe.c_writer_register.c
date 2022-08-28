static const Writer *registered_writers[MAX_REGISTERED_WRITERS_NB + 1];
static int writer_register(const Writer *writer)
{
    static int next_registered_writer_idx = 0;
    if (next_registered_writer_idx == MAX_REGISTERED_WRITERS_NB)
        return AVERROR(ENOMEM);
    registered_writers[next_registered_writer_idx++] = writer;
    return 0;
}
