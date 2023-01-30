static void print(AVTreeNode *t, int depth)
{
    int i;
    for (i = 0; i < depth * 4; i++)
        av_log(NULL, AV_LOG_ERROR, " ");
    if (t) {
        av_log(NULL, AV_LOG_ERROR, "Node %p %2d %p\n", t, t->state, t->elem);
        print(t->child[0], depth + 1);
        print(t->child[1], depth + 1);
    } else
        av_log(NULL, AV_LOG_ERROR, "NULL\n");
}
