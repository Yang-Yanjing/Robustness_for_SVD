 *****************************************************************************/
static bool CAPMTNeedsDescrambling( dvbpsi_pmt_t *p_pmt )
{
    dvbpsi_descriptor_t *p_dr;
    dvbpsi_pmt_es_t *p_es;
    for( p_dr = p_pmt->p_first_descriptor; p_dr != NULL; p_dr = p_dr->p_next )
    {
        if( p_dr->i_tag == 0x9 )
        {
            return true;
        }
    }
    for( p_es = p_pmt->p_first_es; p_es != NULL; p_es = p_es->p_next )
    {
        for( p_dr = p_es->p_first_descriptor; p_dr != NULL;
             p_dr = p_dr->p_next )
        {
            if( p_dr->i_tag == 0x9 )
            {
                return true;
            }
        }
    }
    return false;
}
