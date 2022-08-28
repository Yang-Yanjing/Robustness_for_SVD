int main(int argc, char **argv)
{
    char fntemplate[FILENAME_BUF_SIZE];
    char pktfilename[FILENAME_BUF_SIZE];
    AVFormatContext *fctx = NULL;
    AVPacket pkt;
    int64_t pktnum  = 0;
    int64_t maxpkts = 0;
    int donotquit   = 0;
    int nowrite     = 0;
    int err;
    if ((argc > 1) && !strncmp(argv[1], "-", 1)) {
        if (strchr(argv[1], 'w'))
            donotquit = 1;
        if (strchr(argv[1], 'n'))
            nowrite = 1;
        argv++;
        argc--;
    }
    if (argc < 2)
        return usage(1);
    if (argc > 2)
        maxpkts = atoi(argv[2]);
    av_strlcpy(fntemplate, argv[1], sizeof(fntemplate));
    if (strrchr(argv[1], '/'))
        av_strlcpy(fntemplate, strrchr(argv[1], '/') + 1, sizeof(fntemplate));
    if (strrchr(fntemplate, '.'))
        *strrchr(fntemplate, '.') = '\0';
    if (strchr(fntemplate, '%')) {
        fprintf(stderr, "cannot use filenames containing '%%'\n");
        return usage(1);
    }
    if (strlen(fntemplate) + sizeof(PKTFILESUFF) >= sizeof(fntemplate) - 1) {
        fprintf(stderr, "filename too long\n");
        return usage(1);
    }
    strcat(fntemplate, PKTFILESUFF);
    printf("FNTEMPLATE: '%s'\n", fntemplate);
    
    av_register_all();
    err = avformat_open_input(&fctx, argv[1], NULL, NULL);
    if (err < 0) {
        fprintf(stderr, "cannot open input: error %d\n", err);
        return 1;
    }
    err = avformat_find_stream_info(fctx, NULL);
    if (err < 0) {
        fprintf(stderr, "avformat_find_stream_info: error %d\n", err);
        return 1;
    }
    av_init_packet(&pkt);
    while ((err = av_read_frame(fctx, &pkt)) >= 0) {
        int fd;
        snprintf(pktfilename, sizeof(pktfilename), fntemplate, pktnum,
                 pkt.stream_index, pkt.pts, pkt.size,
                 (pkt.flags & AV_PKT_FLAG_KEY) ? 'K' : '_');
        printf(PKTFILESUFF "\n", pktnum, pkt.stream_index, pkt.pts, pkt.size,
               (pkt.flags & AV_PKT_FLAG_KEY) ? 'K' : '_');
        if (!nowrite) {
            fd  = open(pktfilename, O_WRONLY | O_CREAT, 0644);
            err = write(fd, pkt.data, pkt.size);
            if (err < 0) {
                fprintf(stderr, "write: error %d\n", err);
                return 1;
            }
            close(fd);
        }
        av_free_packet(&pkt);
        pktnum++;
        if (maxpkts && (pktnum >= maxpkts))
            break;
    }
    avformat_close_input(&fctx);
    while (donotquit)
        av_usleep(60 * 1000000);
    return 0;
}
