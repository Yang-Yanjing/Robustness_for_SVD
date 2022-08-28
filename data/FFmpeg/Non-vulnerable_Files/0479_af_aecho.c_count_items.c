AVFILTER_DEFINE_CLASS(aecho);
static void count_items(char *item_str, int *nb_items)
{
    char *p;
    *nb_items = 1;
    for (p = item_str; *p; p++) {
        if (*p == '|')
            (*nb_items)++;
    }
}
