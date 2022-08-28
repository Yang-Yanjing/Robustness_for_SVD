static void print_benchs(CheckasmFunc *f)
{
    if (f) {
        print_benchs(f->child[0]);
        
        if (f->versions.cpu || f->versions.next) {
            CheckasmFuncVersion *v = &f->versions;
            do {
                if (v->iterations) {
                    int decicycles = (10*v->cycles/v->iterations - state.nop_time) / 4;
                    printf("%s_%s: %d.%d\n", f->name, cpu_suffix(v->cpu), decicycles/10, decicycles%10);
                }
            } while ((v = v->next));
        }
        print_benchs(f->child[1]);
    }
}
