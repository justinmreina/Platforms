#include "gpio.h"

#include <sys/time.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define OUT (1)
#define IN  (0)


int main(void) {
	
	int i = 0;
	printf("hello world\n");

	for(i=0; i<28; i++) {
		gpio_set_dir(28, IN);
	}

	int someVal;
	for(i=0; i<28; i++) {
		int someOtherVal = gpio_get_value(i, &someVal);
		printf("Pin[%d]: %d\n", i, someOtherVal);
	}

	printf("\n\n");
	
	return EXIT_SUCCESS;
}

