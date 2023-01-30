}
void dialog_ProgressSet (dialog_progress_bar_t *dialog, const char *text,
                         float value)
{
    assert (dialog);
    dialog->pf_update (dialog->p_sys, text, value);
}
