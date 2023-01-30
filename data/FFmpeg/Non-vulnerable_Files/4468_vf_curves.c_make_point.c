static struct keypoint *make_point(double x, double y, struct keypoint *next)
{
    struct keypoint *point = av_mallocz(sizeof(*point));
    if (!point)
        return NULL;
    point->x = x;
    point->y = y;
    point->next = next;
    return point;
}
