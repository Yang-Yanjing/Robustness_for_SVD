#ifdef HAVE_DVBPSI
void dvb_set_ca_pmt (dvb_device_t *d, struct dvbpsi_pmt_s *pmt)
{
    if (d->cam != NULL)
        en50221_SetCAPMT (d->cam, pmt);
}
