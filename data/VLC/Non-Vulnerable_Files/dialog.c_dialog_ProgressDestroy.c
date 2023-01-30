}
void dialog_ProgressDestroy (dialog_progress_bar_t *dialog)
{
    assert (dialog);
    dialog->pf_destroy (dialog->p_sys);
    free (dialog);
}
