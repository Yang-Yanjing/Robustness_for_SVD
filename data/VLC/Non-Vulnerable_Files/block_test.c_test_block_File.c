    "This file can be deleted safely!\n";
static void test_block_File (void)
{
    FILE *stream;
    int res;
    stream = fopen ("testfile.txt", "wb+e");
    assert (stream != NULL);
    res = fputs (text, stream);
    assert (res != EOF);
    res = fflush (stream);
    assert (res != EOF);
    block_t *block = block_File (fileno (stream));
    fclose (stream);
    assert (block != NULL);
    assert (block->i_buffer == strlen (text));
    assert (!memcmp (block->p_buffer, text, block->i_buffer));
    block_Release (block);
    remove ("testfile.txt");
}
