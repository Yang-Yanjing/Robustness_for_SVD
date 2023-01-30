static void destroy_func_tree(CheckasmFunc *f)
{
    if (f) {
        CheckasmFuncVersion *v = f->versions.next;
        while (v) {
            CheckasmFuncVersion *next = v->next;
            free(v);
            v = next;
        }
        destroy_func_tree(f->child[0]);
        destroy_func_tree(f->child[1]);
        free(f);
    }
}
