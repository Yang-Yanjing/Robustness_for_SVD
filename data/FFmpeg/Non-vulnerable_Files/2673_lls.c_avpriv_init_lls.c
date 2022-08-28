av_cold void avpriv_init_lls(LLSModel *m, int indep_count)
{
    memset(m, 0, sizeof(LLSModel));
    m->indep_count = indep_count;
    m->update_lls = update_lls;
    m->evaluate_lls = evaluate_lls;
    if (ARCH_X86)
        ff_init_lls_x86(m);
}
