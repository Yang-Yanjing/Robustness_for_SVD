}
bool scan_IsCancelled( scan_t *p_scan )
{
    return p_scan->p_dialog && dialog_ProgressCancelled( p_scan->p_dialog );
}
