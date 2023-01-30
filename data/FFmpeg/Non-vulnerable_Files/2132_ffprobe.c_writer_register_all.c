static void writer_register_all(void)
{
    static int initialized;
    if (initialized)
        return;
    initialized = 1;
    writer_register(&default_writer);
    writer_register(&compact_writer);
    writer_register(&csv_writer);
    writer_register(&flat_writer);
    writer_register(&ini_writer);
    writer_register(&json_writer);
    writer_register(&xml_writer);
}
