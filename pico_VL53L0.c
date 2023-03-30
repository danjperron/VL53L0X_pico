//
// Time of Flight sensor test program
//
// Copyright (c) 2017 Larry Bank
// email: bitbank@pobox.com
// Project started 7/29/2017
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/cyw43_arch.h"
#include "tof.h" // time of flight sensor library
#include "hardware/i2c.h"

int main(int argc, char *argv[])
{
int i;
int iDistance;
int model, revision;

  stdio_init_all();


    // initialize wifi
    if (cyw43_arch_init()) {
        printf("Init failed!\n");
        return 1;
    }
    cyw43_pm_value(CYW43_NO_POWERSAVE_MODE,200,1,1,10);
    cyw43_arch_enable_sta_mode();

    printf("WiFi ... ");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed!\n");
        return 1;
    } else {
        printf("Connected.n");
        printf("IP: %s\n",ipaddr_ntoa(((const ip_addr_t *)&cyw43_state.netif[0].ip_addr)));
    }


    // set I2C pins
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));


	i = tofGetModel(&model, &revision);
	printf("Model ID - %d\n", model);
	printf("Revision ID - %d\n", revision);


	// For Raspberry Pi's, the I2C channel is usually 1
	// For other boards (e.g. OrangePi) it's 0
//	i = tofInit(0, 0x29, 1); // set long range mode (up to 2m)
	i = tofInit(0, 0x29, 0);
	if (i != 1)
	{
		return -1; // problem - quit
	}
	printf("VL53L0X device successfully opened.\n");

	for (i=0; i<120000; i++) // read values 20 times a second for 1 minute
	{
		iDistance = tofReadDistance();
		if ((iDistance < 4096) && ( iDistance > 0))// valid range?
			printf("\nDistance = %dmm\n", iDistance);
        else
            printf(".");
		sleep_ms(50); // 50ms
	}

return 0;
} /* main() */
