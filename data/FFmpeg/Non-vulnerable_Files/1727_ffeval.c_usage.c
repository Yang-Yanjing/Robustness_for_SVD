static void usage(void)
{
    printf("Simple expression evalutor, please *don't* turn me to a feature-complete language interpreter\n");
    printf("usage: ffeval [OPTIONS]\n");
    printf("\n"
           "Options:\n"
           "-e                echo each input line on output\n"
           "-h                print this help\n"
           "-i INFILE         set INFILE as input file, stdin if omitted\n"
           "-o OUTFILE        set OUTFILE as output file, stdout if omitted\n"
           "-p PROMPT         set output prompt\n");
}
