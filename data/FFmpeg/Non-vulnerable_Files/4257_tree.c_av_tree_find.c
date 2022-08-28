void *av_tree_find(const AVTreeNode *t, void *key,
                   int (*cmp)(void *key, const void *b), void *next[2])
{
    if (t) {
        unsigned int v = cmp(key, t->elem);
        if (v) {
            if (next)
                next[v >> 31] = t->elem;
            return av_tree_find(t->child[(v >> 31) ^ 1], key, cmp, next);
        } else {
            if (next) {
                av_tree_find(t->child[0], key, cmp, next);
                av_tree_find(t->child[1], key, cmp, next);
            }
            return t->elem;
        }
    }
    return NULL;
}
