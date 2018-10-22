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

#define LOOP_TIME_MS 3000
#define MICROS_TO_MILIS 1000
#define SLEEP_TIME_MS 20000

DigitalOut led1(p22);

uint64_t prev_idle_time = 0;
uint64_t prev_uptime = 0;
uint64_t prev_sleep_time = 0;
uint64_t prev_deep_sleep_time = 0;

int32_t wait_time = 1000;

void busy_thread()
{
    wait_ms(wait_time);
    for(int i=~0; i>0; i--)
	{
	// Just loop for some time
		wait_ms(1);
	}
}

void test_deep_sleep_lock()
{	  
	{
	    	DeepSleepLock lock;     	 	
	 	printf("\nDeep sleep lock object is created.\r\n");
		
		// Code in this block will run with the deep sleep mode locked	
	  	 
		ThisThread::sleep_for(SLEEP_TIME_MS);
	}
	printf("Deep lock object is restored.\r\n");
	ThisThread::sleep_for(SLEEP_TIME_MS);
}

void test_sleep_manager(){
	int ret;
	ThisThread::sleep_for(SLEEP_TIME_MS);

	// check whether deep sleep is allowed
    	ret = sleep_manager_can_deep_sleep();
    	printf("\nDeep sleep allowed: %d\r\n",ret);
	
	ThisThread::sleep_for(SLEEP_TIME_MS);
	
	// lock deep sleep
	sleep_manager_lock_deep_sleep(); 
	printf("Lock deep sleep.\r\n"); 
	
	ThisThread::sleep_for(SLEEP_TIME_MS);
	
	// check whether deep sleep is allowed
    	ret = sleep_manager_can_deep_sleep();
    	printf("Deep sleep allowed: %d\r\n",ret);
}

void print_stats()
{
    uint64_t diff_idle_time;
    uint64_t diff_uptime;
    uint64_t diff_sleep_time;
    uint64_t diff_deep_sleep_time;
    uint64_t current_uptime;
    uint64_t current_idle_time;
    uint64_t current_sleep_time;
    uint64_t current_deep_sleep_time;
    uint8_t  idle_pec;
    uint8_t  up_pec;
    uint8_t  sleep_pec;
    uint8_t  deep_sleep_pec;

    led1 = !led1;
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);
    
    current_uptime              = stats.uptime/MICROS_TO_MILIS;
    current_idle_time           = stats.idle_time/MICROS_TO_MILIS;
    current_sleep_time          = stats.sleep_time/MICROS_TO_MILIS;
    current_deep_sleep_time     = stats.deep_sleep_time/MICROS_TO_MILIS;

    diff_uptime         = current_uptime - prev_uptime;
    diff_idle_time      = current_idle_time - prev_idle_time;
    diff_sleep_time     = current_sleep_time - prev_sleep_time;
    diff_deep_sleep_time        = current_deep_sleep_time - prev_deep_sleep_time;

    up_pec = diff_uptime*100/diff_uptime;
    idle_pec = diff_idle_time*100/diff_uptime;
    sleep_pec = diff_sleep_time*100/diff_uptime;
    deep_sleep_pec = diff_deep_sleep_time*100/diff_uptime;

    printf("%-20lld\t",current_uptime);
    printf("%-20lld\t",current_idle_time);
    printf("%-20lld\t",current_sleep_time);
    printf("%-20lld\t",current_deep_sleep_time);

    printf("| %-20lld",diff_uptime);
    printf("%-20lld",diff_idle_time);
    printf("%-20lld",diff_sleep_time);
    printf("%-20lld",diff_deep_sleep_time);

    printf("| %d %d %d %d | Useage: %d\r\n",
                    up_pec,
                    idle_pec,
                    sleep_pec,
                    deep_sleep_pec,
                    100-idle_pec);

    prev_idle_time = current_idle_time;
    prev_uptime = current_uptime;
    prev_sleep_time = current_sleep_time;
    prev_deep_sleep_time = current_deep_sleep_time;
 }

int main()
{  
    EventQueue *stats_queue = mbed_event_queue();
    Thread *thread;
    int id;
    id = stats_queue->call_every(SAMPLE_TIME_MS, print_stats);
    
    printf("%-20s%-20s%-20s%-20s\r\n", "Uptime[ms]", "Idle Time[ms]",
		"Sleep time[ms]", "DeepSleep time[ms]");
    printf("%-20s%-20s%-20s%-20s\r\n", "Period Uptime[ms]", "Period Idle Time[ms]",  
		"Period Sleep time[ms]", "Period DeepSleep time[ms]");
    printf("%-20s%-20s%-20s%-20s\r\n", "Uptime percentage[%]", "Idle percentage[%]",  
		"Slieep percentage[%]", "DeepSleep percentage[%]");
    
    thread = new Thread(osPriorityNormal, 1024);
    
    //thread->start(test_deep_sleep_lock);
    thread->start(test_sleep_manager);
    
    stats_queue->dispatch_forever();

    printf("Test is over.\n");
    thread->terminate();
    stats_queue->cancel(id);
    return 0;
}
