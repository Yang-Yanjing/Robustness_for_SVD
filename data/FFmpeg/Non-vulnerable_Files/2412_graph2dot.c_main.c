int main(int argc, char **argv)
{
    const char *outfilename = NULL;
    const char *infilename  = NULL;
    FILE *outfile           = NULL;
    FILE *infile            = NULL;
    char *graph_string      = NULL;
    AVFilterGraph *graph = av_mallocz(sizeof(AVFilterGraph));
    char c;
    av_log_set_level(AV_LOG_DEBUG);
    while ((c = getopt(argc, argv, "hi:o:")) != -1) {
        switch (c) {
        case 'h':
            usage();
            return 0;
        case 'i':
            infilename = optarg;
            break;
        case 'o':
            outfilename = optarg;
            break;
        case '?':
            return 1;
        }
    }
    if (!infilename || !strcmp(infilename, "-"))
        infilename = "/dev/stdin";
    infile = fopen(infilename, "r");
    if (!infile) {
        fprintf(stderr, "Failed to open input file '%s': %s\n",
                infilename, strerror(errno));
        return 1;
    }
    if (!outfilename || !strcmp(outfilename, "-"))
        outfilename = "/dev/stdout";
    outfile = fopen(outfilename, "w");
    if (!outfile) {
        fprintf(stderr, "Failed to open output file '%s': %s\n",
                outfilename, strerror(errno));
        return 1;
    }
    
    {
        int64_t count = 0;
        struct line *line, *last_line, *first_line;
        char *p;
        last_line = first_line = av_malloc(sizeof(struct line));
        if (!last_line) {
            fprintf(stderr, "Memory allocation failure\n");
            return 1;
        }
        while (fgets(last_line->data, sizeof(last_line->data), infile)) {
            struct line *new_line = av_malloc(sizeof(struct line));
            if (!new_line) {
                fprintf(stderr, "Memory allocation failure\n");
                return 1;
            }
            count += strlen(last_line->data);
            last_line->next = new_line;
            last_line       = new_line;
        }
        last_line->next = NULL;
        graph_string = av_malloc(count + 1);
        if (!graph_string) {
            fprintf(stderr, "Memory allocation failure\n");
            return 1;
        }
        p = graph_string;
        for (line = first_line; line->next; line = line->next) {
            size_t l = strlen(line->data);
            memcpy(p, line->data, l);
            p += l;
        }
        *p = '\0';
    }
    avfilter_register_all();
    if (avfilter_graph_parse(graph, graph_string, NULL, NULL, NULL) < 0) {
        fprintf(stderr, "Failed to parse the graph description\n");
        return 1;
    }
    if (avfilter_graph_config(graph, NULL) < 0)
        return 1;
    print_digraph(outfile, graph);
    fflush(outfile);
    return 0;
}
