}
bool dialog_ProgressCancelled (dialog_progress_bar_t *dialog)
{
    assert (dialog);
    return dialog->pf_check (dialog->p_sys);
}
