#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <mosquitto.h>
#include "MQTTSub.h"

#define mqtt_host "localhost"
#define mqtt_port 1883

static int run = 1;

void handle_signal(int s)
{
	run = 0;
}

void connect_callback(struct mosquitto *mosqSub, void *obj, int result)
{
	printf("connect callback, rc=%d\n", result);
}

void message_callback(struct mosquitto *mosqSub, void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

	mosquitto_topic_matches_sub("MyHomeJC/Slave/CMD/", message->topic, &match);
	if (match) {
		printf("got message for ADC topic\n");
	}

}
									    
int mqtt_subscribe()
{
	uint8_t reconnect = true;
        char clientid[24];
        struct mosquitto *mosqSub;
        int rc = 0;

        signal(SIGINT, handle_signal);
        signal(SIGTERM, handle_signal);

        mosquitto_lib_init();

        memset(clientid, 0, 24);
        snprintf(clientid, 23, "mysql_log_%d", getpid());
        mosq = mosquitto_new(clientid, true, 0);

        if(mosqSub){
                mosquitto_connect_callback_set(mosqSub, connect_callback);
                mosquitto_message_callback_set(mosqSub, message_callback);

            rc = mosquitto_connect(mosqSub, mqtt_host, mqtt_port, 60);

                mosquitto_subscribe(mosqSub, NULL, "MyHomeJC/Slave/CMD/", 0);

                while(run){
                        rc = mosquitto_loop(mosqSub, -1, 1);
                        if(run && rc){
                                printf("connection error!\n");
                                sleep(10);
                                mosquitto_reconnect(mosqSub);
                        }
                }
                mosquitto_destroy(mosqSub);
        }

        mosquitto_lib_cleanup();

        return rc;	
	
}
