#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <RF24/RF24.h>

#include <sys/types.h>
#include <unistd.h>

#include <mosquitto.h>
#include "MQTTSub.h"
#include <string.h>


#define mqtt_host "localhost"
#define mqtt_port 1883

static int run = 1;

struct package
{
//        unsigned long command1 = 68;
        unsigned long command = 69;
        unsigned long  macAddr = 14;
        unsigned long volts = 11;
        unsigned long amps = 22;
        unsigned long watts = 33;
        unsigned long kwh = 44;
        unsigned long kwh5SecTransmit = 55;
}RFdata;

void handle_signal(int s)
{
        run = 0;
}

void connect_callback(struct mosquitto *mosqSub, void *obj, int result)
{
        printf("connect callback, rc=%d\n", result);
}

struct mosquitto *mqtt_subscribe()
{
        printf("Running Subscribe 1..\n");
        uint8_t reconnect = true;
        char clientid[24];
        struct mosquitto *mosqSub;
        int rc = 0;

        signal(SIGINT, handle_signal);
        signal(SIGTERM, handle_signal);

        mosquitto_lib_init();

        memset(clientid, 0, 24);
        snprintf(clientid, 23, "mysql_log_%d", getpid());
        mosqSub = mosquitto_new(clientid, true, 0);

        if(mosqSub){
                printf("Running Subscribe 2..\n");
                mosquitto_connect_callback_set(mosqSub, connect_callback);
                mosquitto_message_callback_set(mosqSub, message_callback);

            rc = mosquitto_connect(mosqSub, mqtt_host, mqtt_port, 60);

                mosquitto_subscribe(mosqSub, NULL, "ProLighting/Slave/CMD/#", 0);

//                while(run){
//                      printf("Running Subscribe 3..\n");
//                        rc = mosquitto_loop(mosqSub, -1, 1);
//                        if(run && rc){
//                                printf("connection error!\n");
//                                sleep(1);
//                                mosquitto_reconnect(mosqSub);
//                        }
//                }
//                mosquitto_destroy(mosqSub);
        }

//        mosquitto_lib_cleanup();

//        return rc;
          return mosqSub;

}
