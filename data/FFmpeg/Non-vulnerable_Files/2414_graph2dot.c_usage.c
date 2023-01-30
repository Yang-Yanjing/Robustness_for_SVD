static void usage(void)
{
    printf("Convert a libavfilter graph to a dot file.\n");
    printf("Usage: graph2dot [OPTIONS]\n");
    printf("\n"
           "Options:\n"
           "-i INFILE         set INFILE as input file, stdin if omitted\n"
           "-o OUTFILE        set OUTFILE as output file, stdout if omitted\n"
           "-h                print this help\n");
}
