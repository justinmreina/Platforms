/**
 * Title:   press_duration.c
 *
 * Author:  Andrew Montag
 *          ajmontag@gmail.com
 *          sites.google.com/site/andrewmontag
 *
 * Licence: Boost Software Licence - Verison 1.0
 *          http://www.boost.org/users/license.html
 *
 * Purpose:
 * A program which listens for button presses on a GPIO pin and reports the
 * press duration.
 *
 */

#include "gpio.h"

#include <sys/time.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define POLL_TIMEOUT_30_SEC (30 * 1000)
#define POLL_TIMEOUT_20_SEC (20 * 1000)
#define POLL_TIMEOUT_10_SEC (10 * 1000)
#define POLL_TIMEOUT_5_SEC  (5  * 1000)
#define POLL_TIMEOUT_2_SEC  (2  * 1000)
#define POLL_TIMEOUT_1_SEC  (1  * 1000)
#define POLL_TIMEOUT_INF    (-1)

#define MAX_BUF 64

void report_button_ms(int duration) {
    printf("button pressed for %d ms.\n", duration);
}

int main(int argc, char **argv, char **envp)
{
    struct pollfd pollfdset;
    const int nfds = 1;
    int timeout, rc;
    char *buf[MAX_BUF];
    unsigned int gpio;
    int len;

    // gpio pin number
    if (argc < 2) {
        printf("Usage: gpio-int <gpio-pin>\n\n");
        printf("Reports button press duration on a gpio pin\n");
        exit(-1);
    }

    gpio = atoi(argv[1]);

    // export and configure the pin for our usage
    gpio_export(gpio);
    gpio_set_dir(gpio, GPIO_DIR_INPUT);

    // stuff the poll structure
    memset((void*) &pollfdset, 0, sizeof(pollfdset));
    pollfdset.fd = gpio_fd_open(gpio);
    pollfdset.events = POLLPRI;

    // clear any backed up events
    // XXX I'm not sure why we need this, but without it we get a false
    // report of an edge from the first poll()
    rc = poll(&pollfdset, nfds, 0);
    len = read(pollfdset.fd, buf, MAX_BUF);

    while (1) {
        //printf("waiting for rising edge.\n");

        // wait for rising edge, button press
        pollfdset.revents = 0;
        gpio_set_edge(gpio, kPollEdge_rising);
        rc = poll(&pollfdset, nfds, POLL_TIMEOUT_INF);
        len = read(pollfdset.fd, buf, MAX_BUF);

        if (rc < 0) {
            // TODO error
            printf("\npoll() failed!\n");
            return -1;
        } /* else if (rc == 0) timeout */

        if (pollfdset.revents & POLLPRI) {
            // rising edge occurred
            printf("rising edge!\n");

            // make note of the time of the rising edge
            struct timeval starttv;
            gettimeofday(&starttv, NULL);

            // now wait for the falling edge
            gpio_set_edge(gpio, kPollEdge_falling);

            // the button is currently being held down
            int duration = -1;
            while (duration < 0) {
                //printf("waiting for falling edge\n");
                pollfdset.revents = 0;
                rc = poll(&pollfdset, nfds, POLL_TIMEOUT_5_SEC);
                len = read(pollfdset.fd, buf, MAX_BUF);

                if (rc < 0) {
                    printf("error!\n");
                    // TODO error
                    duration = 0;
                } else if (0 == rc) {
                    // timeout, check the state of the button to make sure we didn't miss the edge
                    printf("timeout while waiting for falling edge\n");

                    unsigned int value = 0xFFFF;
                    rc = gpio_get_value_fd(pollfdset.fd, &value);
                    if (0 == value) {
                        // button is released, but we missed the falling edge
                        // report a very short duration
                        printf("Button falling edge missed!\n"); // debug only
                        duration = 1;
                    } else if (1 == value) {
                        // the button is still pressed, carry on
                        printf("Button still pressed\n");
                    }

                    if (rc < 0) {
                        printf("error2!");
                        // TODO error
                        duration = 0;
                    }
                }

                if (pollfdset.revents & POLLPRI) {
                    // falling edge occurred
                    printf("falling edge!\n");

                    // make note of the falling edge time
                    struct timeval endtv;
                    gettimeofday(&endtv, NULL);

                    // subtract the times and convert millisecond
                    struct timeval difftv;
                    timersub(&endtv, &starttv, &difftv);
                    duration = (difftv.tv_sec) * 1000 + (difftv.tv_usec) / 1000 ;
                    assert(duration >= 0); // XXX assert?
                }
            } // end while duration < 0

            report_button_ms(duration);

        }

    }

    // TODO unexport and put in the SIGTERM handler
    gpio_fd_close(pollfdset.fd);
    return 0;
}