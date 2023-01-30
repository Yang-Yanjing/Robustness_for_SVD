static void fill_items(char *item_str, int *nb_items, float *items)
{
    char *p, *saveptr = NULL;
    int i, new_nb_items = 0;
    p = item_str;
    for (i = 0; i < *nb_items; i++) {
        char *tstr = av_strtok(p, "|", &saveptr);
        p = NULL;
        new_nb_items += sscanf(tstr, "%f", &items[i]) == 1;
    }
    *nb_items = new_nb_items;
}
