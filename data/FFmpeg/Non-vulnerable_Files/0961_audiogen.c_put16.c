FILE *outfile;
static void put16(int16_t v)
{
    fputc( v       & 0xff, outfile);
    fputc((v >> 8) & 0xff, outfile);
}
