#ifdef __cplusplus
extern "C" {
#endif

void mosq_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str);
int mqtt_send(char *msg);
void mqtt_setup();


#ifdef __cplusplus
} // closing brace for extern "C"

#endif

