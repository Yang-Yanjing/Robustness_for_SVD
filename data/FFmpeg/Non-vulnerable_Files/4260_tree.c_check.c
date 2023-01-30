static int check(AVTreeNode *t)
{
    if (t) {
        int left  = check(t->child[0]);
        int right = check(t->child[1]);
        if (left > 999 || right > 999)
            return 1000;
        if (right - left != t->state)
            return 1000;
        if (t->state > 1 || t->state < -1)
            return 1000;
        return FFMAX(left, right) + 1;
    }
    return 0;
}
