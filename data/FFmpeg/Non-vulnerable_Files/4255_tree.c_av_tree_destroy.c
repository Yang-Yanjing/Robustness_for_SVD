void av_tree_destroy(AVTreeNode *t)
{
    if (t) {
        av_tree_destroy(t->child[0]);
        av_tree_destroy(t->child[1]);
        av_free(t);
    }
}
