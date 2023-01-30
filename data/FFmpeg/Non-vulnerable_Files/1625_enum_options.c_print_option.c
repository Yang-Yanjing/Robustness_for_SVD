static void print_option(const AVClass *class, const AVOption *o)
{
    printf("@item -%s @var{", o->name);
    switch (o->type) {
    case FF_OPT_TYPE_BINARY:   printf("hexadecimal string"); break;
    case FF_OPT_TYPE_STRING:   printf("string");             break;
    case FF_OPT_TYPE_INT:
    case FF_OPT_TYPE_INT64:    printf("integer");            break;
    case FF_OPT_TYPE_FLOAT:
    case FF_OPT_TYPE_DOUBLE:   printf("float");              break;
    case FF_OPT_TYPE_RATIONAL: printf("rational number");    break;
    case FF_OPT_TYPE_FLAGS:    printf("flags");              break;
    default:                   printf("value");              break;
    }
    printf("} (@emph{");
    if (o->flags & AV_OPT_FLAG_ENCODING_PARAM) {
        printf("input");
        if (o->flags & AV_OPT_FLAG_ENCODING_PARAM)
            printf("/");
    }
    if (o->flags & AV_OPT_FLAG_ENCODING_PARAM)
        printf("output");
    printf("})\n");
    if (o->help)
        printf("%s\n", o->help);
    if (o->unit) {
        const AVOption *u = NULL;
        printf("\nPossible values:\n@table @samp\n");
        while ((u = av_next_option(&class, u)))
            if (u->type == FF_OPT_TYPE_CONST && u->unit && !strcmp(u->unit, o->unit))
                printf("@item %s\n%s\n", u->name, u->help ? u->help : "");
        printf("@end table\n");
    }
}
