static void put32(uint32_t v)
{
    fputc( v        & 0xff, outfile);
    fputc((v >>  8) & 0xff, outfile);
    fputc((v >> 16) & 0xff, outfile);
    fputc((v >> 24) & 0xff, outfile);
}
