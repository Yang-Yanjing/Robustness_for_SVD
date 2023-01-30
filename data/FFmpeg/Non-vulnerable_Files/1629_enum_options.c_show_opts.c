static void show_opts(const AVClass *class)
{
    const AVOption *o = NULL;
    printf("@table @option\n");
    while ((o = av_next_option(&class, o)))
        if (o->type != FF_OPT_TYPE_CONST)
            print_option(class, o);
    printf("@end table\n");
}
