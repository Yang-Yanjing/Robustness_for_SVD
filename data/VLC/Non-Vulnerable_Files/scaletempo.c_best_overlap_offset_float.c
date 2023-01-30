 *****************************************************************************/
static unsigned best_overlap_offset_float( filter_t *p_filter )
{
    filter_sys_t *p = p_filter->p_sys;
    float *pw, *po, *ppc, *search_start;
    float best_corr = INT_MIN;
    unsigned best_off = 0;
    unsigned i, off;
    pw  = p->table_window;
    po  = p->buf_overlap;
    po += p->samples_per_frame;
    ppc = p->buf_pre_corr;
    for( i = p->samples_per_frame; i < p->samples_overlap; i++ ) {
      *ppc++ = *pw++ * *po++;
    }
    search_start = (float *)p->buf_queue + p->samples_per_frame;
    for( off = 0; off < p->frames_search; off++ ) {
      float corr = 0;
      float *ps = search_start;
      ppc = p->buf_pre_corr;
      for( i = p->samples_per_frame; i < p->samples_overlap; i++ ) {
        corr += *ppc++ * *ps++;
      }
      if( corr > best_corr ) {
        best_corr = corr;
        best_off  = off;
      }
      search_start += p->samples_per_frame;
    }
    return best_off * p->bytes_per_frame;
}
