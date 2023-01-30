 */
static void MainLoopStatistic( input_thread_t *p_input )
{
    stats_ComputeInputStats( p_input, p_input->p->p_item->p_stats );
    input_SendEventStatistics( p_input );
}
