
static int disp_tree(const struct color_node *node, const char *fname)
{
    AVBPrint buf;
    FILE *f = av_fopen_utf8(fname, "w");
    if (!f) {
        int ret = AVERROR(errno);
        av_log(NULL, AV_LOG_ERROR, "Cannot open file '%s' for writing: %s\n",
               fname, av_err2str(ret));
        return ret;
    }
    av_bprint_init(&buf, 0, AV_BPRINT_SIZE_UNLIMITED);
    av_bprintf(&buf, "digraph {\n");
    av_bprintf(&buf, "    node [style=filled fontsize=10 shape=box]\n");
    disp_node(&buf, node, -1, 0, 0);
    av_bprintf(&buf, "}\n");
    fwrite(buf.str, 1, buf.len, f);
    fclose(f);
    av_bprint_finalize(&buf, NULL);
    return 0;
}
