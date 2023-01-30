}
static int ScanDvbCNext( scan_t *p_scan, scan_configuration_t *p_cfg, double *pf_pos )
{
    bool b_servicefound = false;
#ifdef _DVBPSI_DR_44_H_
    /* We iterate frequencies/modulations/symbolrates until we get first hit and find NIT,
       from that we fill pp_service with configurations and after that we iterate over
       pp_services for all that doesn't have name yet (tune to that cfg and get SDT and name
       for channel).
     */
    for( int i = 0; i < p_scan->i_service; i++ )
    {
        /* We found radio/tv config that doesn't have a name,
           lets tune to that mux
         */
        if( !p_scan->pp_service[i]->psz_name && ( p_scan->pp_service[i]->type != SERVICE_UNKNOWN ) )
        {
            p_cfg->i_frequency  = p_scan->pp_service[i]->cfg.i_frequency;
            p_cfg->i_symbolrate = p_scan->pp_service[i]->cfg.i_symbolrate;
            p_cfg->i_modulation = p_scan->pp_service[i]->cfg.i_modulation;
            p_scan->i_index = i+1;
            msg_Dbg( p_scan->p_obj, "iterating to freq: %u, symbolrate %u, "
                     "modulation %u index %"PRId64"/%d",
                     p_cfg->i_frequency, p_cfg->i_symbolrate, p_cfg->i_modulation, p_scan->i_index, p_scan->i_service );
            *pf_pos = (double)i/p_scan->i_service;
            return VLC_SUCCESS;
        }
    }
    /* We should have iterated all channels by now */
    if( p_scan->i_service )
        return VLC_EGENERIC;
#else
    /* fallback to old, so when we get one channe, use that
       symbolrate/modulation until bitter end
     */
    for( int i=0; i < p_scan->i_service; i++ )
    {
        b_servicefound = p_scan->pp_service[i]->type != SERVICE_UNKNOWN;
        if( b_servicefound )
            break;
    }
#endif
    if( !b_servicefound )
    {
        bool b_rotate=true;
        if( !p_scan->parameter.b_modulation_set )
        {
            p_scan->parameter.i_modulation = (p_scan->parameter.i_modulation >> 1 );
            /* if we iterated all modulations, move on */
            /* dvb utils dvb-c channels files seems to have only
               QAM64...QAM256, so lets just iterate over those */
            if( p_scan->parameter.i_modulation < 64)
            {
                p_scan->parameter.i_modulation = 256;
            } else {
                b_rotate=false;
            }
            msg_Dbg( p_scan->p_obj, "modulation %d ", p_scan->parameter.i_modulation);
        }
        if( !p_scan->parameter.b_symbolrate_set )
        {
            /* symbol rates from dvb-tools dvb-c files */
            static const unsigned short symbolrates[] = {
             6900, 6875, 6950
             /* With DR_44 we can cover other symbolrates from NIT-info
                as all channel-seed files have atleast one channel that
                has one of these symbolrate
              */
#ifndef _DVBPSI_DR_44_H_
             ,7000, 3450, 6111,
             6428, 6952, 5900, 5000
#endif
             };
            enum { num_symbols = (sizeof(symbolrates)/sizeof(*symbolrates)) };
            /* if we rotated modulations, rotate symbolrate */
            if( b_rotate )
            {
                p_scan->parameter.i_symbolrate++;
                p_scan->parameter.i_symbolrate %= num_symbols;
            }
            p_cfg->i_symbolrate = 1000 * (symbolrates[ p_scan->parameter.i_symbolrate ] );
            msg_Dbg( p_scan->p_obj, "symbolrate %d", p_cfg->i_symbolrate );
            if( p_scan->parameter.i_symbolrate )
                b_rotate=false;
        }
        if( !b_rotate && p_scan->i_index )
            p_scan->i_index--;
    }
    p_cfg->i_modulation = p_scan->parameter.i_modulation;
    if( !p_cfg->i_symbolrate )
        p_cfg->i_symbolrate = var_GetInteger( p_scan->p_obj, "dvb-srate" );
    if( p_scan->parameter.b_exhaustive )
        return ScanDvbNextExhaustive( p_scan, p_cfg, pf_pos );
    else
        return ScanDvbCNextFast( p_scan, p_cfg, pf_pos );
}
