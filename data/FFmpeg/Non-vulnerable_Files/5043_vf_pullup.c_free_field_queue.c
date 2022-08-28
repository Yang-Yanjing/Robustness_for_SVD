static void free_field_queue(PullupField *head)
{
    PullupField *f = head;
    do {
        PullupField *next;
        if (!f)
            break;
        av_free(f->diffs);
        av_free(f->combs);
        av_free(f->vars);
        next = f->next;
        memset(f, 0, sizeof(*f)); 
        av_free(f);
        f = next;
    } while (f != head);
}
