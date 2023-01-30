static int usage(int ret)
{
    fprintf(stderr, "Dump (up to maxpkts) AVPackets as they are demuxed by libavformat.\n");
    fprintf(stderr, "Each packet is dumped in its own file named like\n");
    fprintf(stderr, "$(basename file.ext)_$PKTNUM_$STREAMINDEX_$STAMP_$SIZE_$FLAGS.bin\n");
    fprintf(stderr, "pktdumper [-nw] file [maxpkts]\n");
    fprintf(stderr, "-n\twrite No file at all, only demux.\n");
    fprintf(stderr, "-w\tWait at end of processing instead of quitting.\n");
    return ret;
}
