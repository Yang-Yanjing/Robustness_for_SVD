static int get_nb_points(const struct keypoint *d)
{
    int n = 0;
    while (d) {
        n++;
        d = d->next;
    }
    return n;
}
