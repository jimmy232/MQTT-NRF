#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <RF24/RF24.h>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define OBSTACLE_PIN 13

#define ADDRESS     "172.20.10.2:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MyHomeJC"
#define PAYLOAD     _txBuffer1
#define QOS         1
#define TIMEOUT     10000L


RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);


const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };

struct package
{
	int command;
	int macAddr;
	unsigned long volts;
	unsigned long amps;
	unsigned long watts;
	unsigned long kwh;
	int kwh5SecTransmit;
}data;


void func(struct package *newData)
{
	printf("\n");
	printf("command2: %d \n", newData->command);
	printf("macAddr2: %d \n", newData->macAddr);
	printf("Volts2: %lu \n", newData->volts);
	printf("Amps2: %lu \n", newData->amps);
	printf("Watts2: %lu \n", newData->watts);
	printf("kwh2: %lu \n", newData->kwh);
}

void runMQTT()
{
	int sw1 = 0;
	sw1 = 1 << 8;
	int sw2 = 0;
	sw2 = 1 << 9;
	bool sw1Stat = sw1 & data.command;
	bool sw2Stat = sw2 & data.command;
	char command[200] ;
	sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/SwitchOne -m %d\n", data.macAddr, sw1Stat);
	system(command);
	sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/SwitchTwo -m %d\n", data.macAddr, sw2Stat);
	system(command);
	sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/volts -m %lu\n", data.macAddr, data.volts);
	system(command);
	sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/amps -m %lu\n", data.macAddr, data.amps);
	system(command);
	sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/watts -m %lu\n", data.macAddr, data.watts);
	system(command);
	sprintf(command, "mosquitto_pub -h 10.0.0.74 -t MyHomeJC/OUT/Slave/%d/kwh -m %lu\n", data.macAddr, data.kwh);
	system(command);
}


int main(int argc, char** argv)
{
	
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
		if ( radio.available() )
		{
			printf("Radio.available()\n");
			while(radio.available())
			{
				radio.read( &data, 32);
			}
			func(&data);
			runMQTT();
			delay(925); //Delay after payload responded to, minimize RPi CPU time
		}
	}
	return 0;

}






