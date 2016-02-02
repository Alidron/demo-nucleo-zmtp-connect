/* Copyright (c) 2015-2016 Contributors as noted in the AUTHORS file
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sys/autostart.h"
#include "sys/etimer.h"
#include "zmq.h"
#include "zmtp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/sensor-common.h"

#include "st-lib.h"

PROCESS(demo, "Connect demo");
AUTOSTART_PROCESSES(&demo);

zmq_socket_t pub, sub;
LIST(read_list);

static const char *uri_sensor_button = "sensor://nucleo-sensor-demo/button";

#define VALUE_UPDATE(sock, uri, value) \
    do{ \
        msg = zmq_msg_from_const_data(0, uri "\0" value, strlen(uri) + 1 +strlen(value)); \
        PT_WAIT_THREAD(process_pt, sock.send(&sock, msg)); \
        zmq_msg_destroy(&msg); \
    }while(0)

#define VALUE_TRIGGER(sock, uri) \
    do{ \
        msg = zmq_msg_from_const_data(0, uri, strlen(uri)); \
        PT_WAIT_THREAD(process_pt, sock.send(&sock, msg)); \
        zmq_msg_destroy(&msg); \
    }while(0)

#define CMP_MSG_STR(msg, s) ( \
    (zmq_msg_size(msg) == strlen(s)) && \
    !strncmp((const char *) zmq_msg_data(msg), s, strlen(s)) \
  )

PROCESS_THREAD(demo, ev, data) {
    static zmq_msg_t *msg = NULL;

    PROCESS_BEGIN();

    SENSORS_ACTIVATE(button_sensor);

    leds_init();

    zmq_init();
    zmq_socket_init(&sub, ZMQ_SUB);
    zmq_setsockopt(&sub, ZMQ_SUBSCRIBE, "");
    zmq_connect(&sub, "aaaa::600:fbff:a2df:5d20", 9999);

    zmq_socket_init(&pub, ZMQ_PUB);
    zmq_connect(&pub, "aaaa::600:fbff:a2df:5d20", 8888);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event) || (ev == zmq_socket_input_activity) || (ev == PROCESS_EVENT_POLL));

        if(ev == PROCESS_EVENT_POLL) {
            int nb_connected=0;

            zmtp_connection_t *conn = list_head(pub.channel.connections);
            while(conn != NULL) {
                if((conn->validated & CONNECTION_VALIDATED) == CONNECTION_VALIDATED) {
                    printf("One PUB connected\r\n");
                    nb_connected++;
                }
                conn = list_item_next(conn);
            }

            conn = list_head(sub.channel.connections);
            while(conn != NULL) {
                if((conn->validated & CONNECTION_VALIDATED) == CONNECTION_VALIDATED) {
                    printf("One SUB connected\r\n");
                    nb_connected++;
                }
                conn = list_item_next(conn);
            }

            if(nb_connected >= 2)
                BSP_LED_Toggle(LED2);

            continue;
        }

        if((ev == sensors_event) && (data == &button_sensor)) {
          printf("Sensor event detected: Button Pressed.\r\n");
          // VALUE_UPDATE(push, "sensor://nucleo-connect-demo/button", "pressed");
          VALUE_TRIGGER(pub, "action://nucleo-sensor-demo/led/blue/toggle");
        }

        if(ev == zmq_socket_input_activity) {
            PT_WAIT_THREAD(process_pt, sub.recv_multipart(&sub, read_list));

            msg = list_pop(read_list);
            if(CMP_MSG_STR(msg, uri_sensor_button))
                leds_toggle(LEDS_RED);

            while(msg != NULL) {
                printf("Received: ");
                uint8_t *data = zmq_msg_data(msg);
                uint8_t *pos = data;
                size_t size = zmq_msg_size(msg);
                while(pos < (data + size))
                    printf("%c", *pos++);
                printf("\r\n");

                zmq_msg_destroy(&msg);
                msg = list_pop(read_list);
            }
        }
    }

    PROCESS_END();
}
