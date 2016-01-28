/* Copyright 2016 - Alidron's authors
 *
 * This file is part of Alidron.
 *
 * Alidron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Alidron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Alidron.  If not, see <http://www.gnu.org/licenses/>.
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
