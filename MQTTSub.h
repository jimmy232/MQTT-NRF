#ifdef __cplusplus
extern "C" {
#endif

void connect_callback(struct mosquitto *mosq, void *obj, int result);
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
struct mosquitto *mqtt_subscribe();

#ifdef __cplusplus
} // closing brace for extern "C"

#endif
