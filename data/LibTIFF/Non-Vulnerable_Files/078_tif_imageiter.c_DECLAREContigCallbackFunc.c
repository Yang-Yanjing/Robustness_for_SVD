DECLAREContigCallbackFunc(TestContigCallback)
{
    printf("Contig Callback called with x = %d, y = %d, w = %d, h = %d, fromskew = %d\n",
	   x, y, w, h, fromskew);
}
