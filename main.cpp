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

uint64_t prev_idle_time = 0;
uint64_t prev_uptime = 0;
uint64_t prev_sleep_time = 0;
uint64_t prev_deep_sleep_time = 0;

#define LOOP_TIME_MS 3000
DigitalOut led1(p22);

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

void test_deep_sleep_lock(){
	  
	{
	    	DeepSleepLock lock;     	 	
	 	printf("\ndeep sleep lock object is created\r\n");
		
		// Code in this block will run with the deep sleep mode locked	
	  	 
		ThisThread::sleep_for(20000);
	}
	printf("deep lock object is restored\r\n");
	ThisThread::sleep_for(20000);
}

void test_sleep_manager(){
	int ret;
    	
	ThisThread::sleep_for(10000);

	// ckeck whether deep sleep is allowed
    	ret = sleep_manager_can_deep_sleep();
    	printf("\ndeep sleep allowed: %d\r\n",ret);
	
	ThisThread::sleep_for(20000);
	
	// lock deep sleep
	sleep_manager_lock_deep_sleep(); 
	printf("lock deep sleep\r\n"); 
	
	ThisThread::sleep_for(20000);
	
	// ckeck whether deep sleep is allowed
    	ret = sleep_manager_can_deep_sleep();
    	printf("deep sleep allowed: %d\r\n",ret);
	return;
}

void print_stats()
{
    led1 = !led1;
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    uint64_t diff_idle_time,
             diff_uptime,
             diff_sleep_time,
             diff_deep_sleep_time,
             current_uptime,
             current_idle_time,
             current_sleep_time,
             current_deep_sleep_time;
    uint8_t idle_pec,
            up_pec,
            sleep_pec,
            deep_sleep_pec;

    current_uptime              = stats.uptime/1000;
    current_idle_time           = stats.idle_time/1000;
    current_sleep_time          = stats.sleep_time/1000;
    current_deep_sleep_time     = stats.deep_sleep_time/1000;

    diff_uptime         = current_uptime - prev_uptime;
    diff_idle_time      = current_idle_time - prev_idle_time;
    diff_sleep_time     = current_sleep_time - prev_sleep_time;
    diff_deep_sleep_time        = current_deep_sleep_time - prev_deep_sleep_time;

    up_pec = diff_uptime*100/diff_uptime;
    idle_pec = diff_idle_time*100/diff_uptime;
    sleep_pec = diff_sleep_time*100/diff_uptime;
    deep_sleep_pec = diff_deep_sleep_time*100/diff_uptime;

    printf("%-20lld",current_uptime);
    printf("%-20lld",current_idle_time);
    printf("%-20lld",current_sleep_time);
    printf("%-20lld",current_deep_sleep_time);

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
  
	// Request the shared queue
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
    
    thread->start(test_deep_sleep_lock);
    //thread->start(test_sleep_manager);
    
    stats_queue->dispatch_forever();

    printf("Test is over.\n");
    thread->terminate();
    stats_queue->cancel(id);
    return 0;
 }
