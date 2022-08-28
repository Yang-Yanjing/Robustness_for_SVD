int main(int argc, char **argv)
{
    const char *basename = NULL;
    const char *path_prefix = "", *ismc_prefix = "";
    const char *output_prefix = "";
    char output_prefix_buf[2048];
    int split = 0, ismf = 0, i;
    struct Tracks tracks = { 0, .video_track = -1, .audio_track = -1 };
    av_register_all();
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n")) {
            basename = argv[i + 1];
            i++;
        } else if (!strcmp(argv[i], "-path-prefix")) {
            path_prefix = argv[i + 1];
            i++;
        } else if (!strcmp(argv[i], "-ismc-prefix")) {
            ismc_prefix = argv[i + 1];
            i++;
        } else if (!strcmp(argv[i], "-output")) {
            output_prefix = argv[i + 1];
            i++;
            if (output_prefix[strlen(output_prefix) - 1] != '/') {
                snprintf(output_prefix_buf, sizeof(output_prefix_buf),
                         "%s/", output_prefix);
                output_prefix = output_prefix_buf;
            }
        } else if (!strcmp(argv[i], "-split")) {
            split = 1;
        } else if (!strcmp(argv[i], "-ismf")) {
            ismf = 1;
        } else if (argv[i][0] == '-') {
            return usage(argv[0], 1);
        } else {
            if (!basename)
                ismf = 0;
            if (handle_file(&tracks, argv[i], split, ismf,
                            basename, output_prefix))
                return 1;
        }
    }
    if (!tracks.nb_tracks || (!basename && !split))
        return usage(argv[0], 1);
    if (!split)
        output_server_manifest(&tracks, basename, output_prefix,
                               path_prefix, ismc_prefix);
    output_client_manifest(&tracks, basename, output_prefix, split);
    clean_tracks(&tracks);
    return 0;
}
