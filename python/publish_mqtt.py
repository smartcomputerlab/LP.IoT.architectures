#!/usr/bin/env python3

import paho.mqtt.client as mqtt

# This is the Publisher

client = mqtt.Client()
client.connect("broker.emqx.io",1883,60)
client.publish("/channel_number", "F1:100,F2:54,F3:10");
client.disconnect();


