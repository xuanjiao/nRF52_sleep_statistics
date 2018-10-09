/**
 * Made by Jurica @ aconno
 * Simple program to demonstrate how to use mbed-os statistics for power
 * consumption tracking
 */

#include "mbed.h"
#include "aconnoConfig.h"

#if !defined(MBED_CPU_STATS_ENABLED) || !defined(DEVICE_LPTICKER) || !defined(DEVICE_SLEEP)
#error [NOT_SUPPORTED] test not supported
#endif

DigitalOut led1(p22);

uint64_t prev_idle_time = 0;
int32_t wait_time = 1000;

void busy_thread()
{
	led1 = !led1;
    wait_ms(wait_time);
 	for(int i=~0; i>0; i--)
	{
		// Just loop for some time
		wait_ms(1);
	}
}

void print_stats()
{
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    printf("%-20lld", stats.uptime/1000);
    printf("%-20lld", stats.idle_time/1000);
    printf("%-20lld", stats.sleep_time/1000);
    printf("%-20lld\r\n", stats.deep_sleep_time/1000);
}

int main()
{
    // Request the shared queue
    EventQueue *stats_queue = mbed_event_queue();
    Thread *thread;
    int id;

    id = stats_queue->call_every(SAMPLE_TIME_MS, print_stats);
    printf("%-20s%-20s%-20s%-20s\r\n", "Uptime[ms]", "Idle Time[ms]",
		"Sleep time[ms]", "DeepSleep time[ms]");

    thread = new Thread(osPriorityNormal, 1024);
    thread->start(busy_thread);

	stats_queue->dispatch_forever();

	printf("Test is over.\n");
    thread->terminate();
    stats_queue->cancel(id);
    return 0;
 }
