#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <RF24/RF24.h>
#include "MQTTPub.h"
#include "MQTTSub.h"
#include <mosquitto.h>


using namespace std;


RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

//extern "C" void mqtt_setup();

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };

struct package
{
//      unsigned long command1;
        unsigned long command = 111;
        unsigned long macAddr;
        unsigned long volts;
        unsigned long amps;
        unsigned long watts;
        unsigned long kwh;
        unsigned long kwh5SecTransmit;
}data;


void func(struct package *newData)
{
        printf("\n");
        printf("command: %d \n", newData->command);
        printf("macAddr: %d \n", newData->macAddr);
        printf("Volts: %lu \n", newData->volts);
        printf("Amps: %lu \n", newData->amps);
        printf("Watts: %lu \n", newData->watts);
        printf("kwh: %lu \n", newData->kwh);
}

void runMQTT()
{
        int sw1 = 0;
        sw1 = 1 << 8;
        int sw2 = 0;
        sw2 = 1 << 9;
        char sw1Stat = sw1 & data.command;
        char sw2Stat = sw2 & data.command;
        char topic[200] ;
        char msg[200];
        char *msg1 = NULL;
        msg1 = &sw1Stat;
        char *msg2 = NULL;
        msg2 = &sw2Stat;
		
		 //sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/SwitchOne -m %d\n", data.macAddr, sw1Stat);
        sprintf(topic, "MyHomeJC/OUT/Slave/Command/%d", data.macAddr);
        sprintf(msg, "%d", data.command);
        mqtt_send(topic, msg);
        sprintf(topic, "MyHomeJC/OUT/Slave/volts/%d", data.macAddr);
        sprintf(msg, "%ul", data.volts);
        mqtt_send(topic, msg);
        sprintf(topic, "MyHomeJC/OUT/Slave/amps/%d", data.macAddr);
        sprintf(msg, "%ul", data.amps);
        mqtt_send(topic, msg);
        sprintf(topic, "MyHomeJC/OUT/Slave/watts/%d", data.macAddr);
        sprintf(msg, "%ul", data.watts);
        mqtt_send(topic, msg);
        sprintf(topic, "MyHomeJC/OUT/Slave/kwh/%d", data.macAddr);
        sprintf(msg, "%ul", data.kwh);
        mqtt_send(topic, msg);

//      system(command);
//      sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/SwitchTwo -m %d\n", data.macAddr, sw2Stat);
//      system(command);
//      sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/volts -m %lu\n", data.macAddr, data.volts);
//      system(command);
//      sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/amps -m %lu\n", data.macAddr, data.amps);
//      system(command);
//      sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/watts -m %lu\n", data.macAddr, data.watts);
//      system(command);
//      sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/kwh -m %lu\n", data.macAddr, data.kwh);
//      system(command);


}

void message_callback(struct mosquitto *mosqSub, void *obj, const struct mosquitto_message *message)
{
//      printf("GET FUCKED!\n");
        bool match = 0;
//        printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

        mosquitto_topic_matches_sub("MyHomeJC/Slave/CMD/#", message->topic, &match);
        if (match) {
//                printf("%s\n", message->topic);
//                printf("Message: %s\n", (char*) message->payload );
        }
        char *ret;
        char *messageRaw;
        char *newMsg = (char*)message->payload;
        char ch = '/';
        const char s[2] = "/";
        char *token;
        ret = (char*)memchr(newMsg, ch, strlen(newMsg));
        char *string = ret + 1;
        data.command = strtoul(string, NULL,0);

        token = strtok(newMsg, s);
        data.macAddr = strtoul(token, NULL,0);

//        printf("RFdata.command:%lu\n", data.command);
//        printf("RFdata.macAddr:%lu\n", data.macAddr);

        radio.stopListening();
        radio.write(&data, sizeof(data));
        radio.startListening();

}

int main(int argc, char** argv)
{
        int rc = 0;

        printf("RUNNING...\n");
        mqtt_setup();
        mqtt_subscribe();
        struct mosquitto *mosqSub = mqtt_subscribe();


        radio.begin();
        radio.setRetries(15,15);
        radio.setPayloadSize(32);
        radio.setPALevel(RF24_PA_MAX);
        radio.setDataRate(RF24_2MBPS);
        radio.printDetails();

        radio.openWritingPipe(pipes[1]);
        radio.openReadingPipe(1,pipes[0]);
        radio.startListening();
        printf("Running Prog:\n");

        while(1)
        {
        rc = mosquitto_loop(mosqSub, -1, 1);
                if ( radio.available() )
                {
//                        printf("Radio.available()\n");
                        while(radio.available())
                        {
                                radio.read( &data, 32);
                        }
                        func(&data);
                        runMQTT();
                        delay(500); // 925 millis Delay after payload responded to, minimize RPi CPU time
                }
        }
        return 0;
}
