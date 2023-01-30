 *****************************************************************************/
static APIRET vlc_DosDevIOCtl( HFILE hdevice, ULONG category, ULONG function,
                               PVOID pParams, ULONG cbParamLenMax,
                               PULONG pcbParamLen, PVOID pData,
                               ULONG cbDataLenMax, PULONG pcbDataLen )
{
    PVOID pParamsLow = NULL;
    PVOID pDataLow = NULL;
    ULONG cbParamLenLow;
    ULONG cbDataLenLow;
    APIRET rc;
    rc = DosAllocMem( &pParamsLow, cbParamLenMax, fALLOC );
    if( rc )
        goto exit_free;
    rc = DosAllocMem( &pDataLow, cbDataLenMax, fALLOC );
    if( rc )
        goto exit_free;
    memcpy( pParamsLow, pParams, cbParamLenMax );
    memcpy( pDataLow, pData, cbDataLenMax );
    cbParamLenLow = *pcbParamLen;
    cbDataLenLow  = *pcbDataLen;
    rc = DosDevIOCtl( hdevice, category, function, pParamsLow,
                      cbParamLenMax, &cbParamLenLow, pDataLow, cbDataLenMax,
                      &cbDataLenLow );
    if( !rc )
    {
        memcpy( pParams, pParamsLow, cbParamLenMax );
        memcpy( pData, pDataLow, cbDataLenMax );
        *pcbParamLen = cbParamLenLow;
        *pcbDataLen  = cbDataLenLow;
    }
exit_free:
    DosFreeMem( pParamsLow);
    DosFreeMem( pDataLow);
    return rc;
}
