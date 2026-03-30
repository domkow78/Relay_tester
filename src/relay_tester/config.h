#ifndef CONFIG_H
#define CONFIG_H

#define FW_VERSION "1.0"

#define CHANNEL_COUNT 10

#define DEFAULT_STEP_DELAY 1000
#define DEFAULT_TARGET_CYCLES 100000
#define DEFAULT_MEASURE_INTERVAL 20000
#define DEFAULT_SAVE_INTERVAL    100   // zapis stanu co 100 cykli (~10x rzadziej niż poprzednio)

#define DEAD_TIME 100

#define RUNTIME_SAVE_INTERVAL   300   // zapis runtime co 5 minut (~5x rzadziej niż poprzednio)

#define DIR_LEFT 0
#define DIR_RIGHT 1

#endif