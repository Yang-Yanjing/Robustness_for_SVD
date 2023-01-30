const int av_tree_node_size = sizeof(AVTreeNode);
struct AVTreeNode *av_tree_node_alloc(void)
{
    return av_mallocz(sizeof(struct AVTreeNode));
}
