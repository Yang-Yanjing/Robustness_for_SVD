int main(int argc, char **argv)
{
    int i;
    void *k;
    AVTreeNode *root = NULL, *node = NULL;
    AVLFG prng;
    int log_level = argc <= 1 ? AV_LOG_INFO : atoi(argv[1]);
    av_log_set_level(log_level);
    av_lfg_init(&prng, 1);
    for (i = 0; i < 10000; i++) {
        intptr_t j = av_lfg_get(&prng) % 86294;
        if (check(root) > 999) {
            av_log(NULL, AV_LOG_ERROR, "FATAL error %d\n", i);
            print(root, 0);
            return 1;
        }
        av_log(NULL, AV_LOG_DEBUG, "inserting %4d\n", (int)j);
        if (!node)
            node = av_tree_node_alloc();
        if (!node) {
            av_log(NULL, AV_LOG_ERROR, "Memory allocation failure.\n");
            return 1;
        }
        av_tree_insert(&root, (void *)(j + 1), cmp, &node);
        j = av_lfg_get(&prng) % 86294;
        {
            AVTreeNode *node2 = NULL;
            av_log(NULL, AV_LOG_DEBUG, "removing %4d\n", (int)j);
            av_tree_insert(&root, (void *)(j + 1), cmp, &node2);
            k = av_tree_find(root, (void *)(j + 1), cmp, NULL);
            if (k)
                av_log(NULL, AV_LOG_ERROR, "removal failure %d\n", i);
            av_free(node2);
        }
    }
    av_free(node);
    av_tree_destroy(root);
    return 0;
}
