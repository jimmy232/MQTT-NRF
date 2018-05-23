
# MQTT-NRF
RPI comms project

//Install the follow libs
sudo apt-get install libmosquitto-dev

//Compile
sudo g++ -o exe MQTTRF.cpp MQTTPub.c MQTTSub.c -lrf24-bcm -lmosquitto

//RF24 lib  - sudo make install
https://github.com/nRF24/RF24.git
