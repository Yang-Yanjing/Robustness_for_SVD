static void show_opts(const AVOption *opts, int per_stream)
{
    const AVOption *o;
    printf("@table @option\n");
    for (o = opts; o->name; o++) {
        if (o->type != AV_OPT_TYPE_CONST)
            print_option(opts, o, per_stream);
    }
    printf("@end table\n");
}
