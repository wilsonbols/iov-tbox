// collision.h
#include "MQTTClient.h"

#ifndef COLLISION_H
#define COLLISION_H

void readcansimulate(MQTTClient client, char *topic);
void mqtt_publish(MQTTClient client, char *topic, char *payload);

#endif  // COLLISION_H