# mbed-os statistics API demonstration program

This simple program demonstrates usage of mbed-os API for CPU statistics (more info about the API here: https://os.mbed.com/docs/v5.9/reference/mbed-statistics.html)

Program prints statistics on stdio every SAMPLE_TIME_MS with the following data:
- Active time (in ms)
- Idle time (in ms)
- Sleep time (in ms)
- Deep sleep time (in ms)

>**Main thread** - The default thread that executes the application's main function. The main thread has 4kB of stack space by default. The application can configure it in mbed_app.json by defining the MAIN_STACK_SIZE parameter.
>**Idle thread** - The thread that's run by the scheduler when there's no other activity in the system (e.g. all other threads are waiting for some event). It's used to make sure the board is not burning empty processor cycles, but is put to sleep for as long as possible.
>**Active time** - Time from startup
>**Idle time** - Time spent in idle thread (Idle time is sleep time + deep sleep time)
>**Awake time** - Time spent in users thread (outside idle and sleep modes: active time - idle time)
