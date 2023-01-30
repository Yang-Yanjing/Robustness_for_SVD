static int adx_encode_header(AVCodecContext *avctx, uint8_t *buf, int bufsize)
{
    ADXContext *c = avctx->priv_data;
    bytestream_put_be16(&buf, 0x8000);              
    bytestream_put_be16(&buf, HEADER_SIZE - 4);     
    bytestream_put_byte(&buf, 3);                   
    bytestream_put_byte(&buf, BLOCK_SIZE);          
    bytestream_put_byte(&buf, 4);                   
    bytestream_put_byte(&buf, avctx->channels);     
    bytestream_put_be32(&buf, avctx->sample_rate);  
    bytestream_put_be32(&buf, 0);                   
    bytestream_put_be16(&buf, c->cutoff);           
    bytestream_put_byte(&buf, 3);                   
    bytestream_put_byte(&buf, 0);                   
    bytestream_put_be32(&buf, 0);                   
    bytestream_put_be32(&buf, 0);                   
    bytestream_put_be16(&buf, 0);                   
    bytestream_put_buffer(&buf, "(c)CRI", 6);       
    return HEADER_SIZE;
}
