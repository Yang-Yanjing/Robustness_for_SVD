int main(int argc, char **argv)
{
    const char *out = NULL;
    struct Tracks tracks = { 0 };
    int i;
    av_register_all();
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-out")) {
            out = argv[i + 1];
            i++;
        } else if (argv[i][0] == '-') {
            return usage(argv[0], 1);
        } else {
            if (handle_file(&tracks, argv[i]))
                return 1;
        }
    }
    if (!tracks.nb_tracks || !out)
        return usage(argv[0], 1);
    output_mpd(&tracks, out);
    clean_tracks(&tracks);
    return 0;
}
