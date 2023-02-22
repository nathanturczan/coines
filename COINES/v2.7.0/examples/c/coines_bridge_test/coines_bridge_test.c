#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "coines.h"

static void check(int8_t res);
void com_echo_test(void);
int16_t coines_echo_test(uint8_t *data, uint16_t length);

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    printf("Serial COM test\r\n");

    check(coines_open_comm_intf(COINES_COMM_INTF_USB, NULL));
    coines_delay_msec(10);

    com_echo_test();

    coines_delay_msec(10);
    check(coines_close_comm_intf(COINES_COMM_INTF_USB, NULL));

    return 0;
}

static void check(int8_t res)
{
    if (res)
    {
        printf("Error %d\r\n", res);
        exit(0);
    }
}

void com_echo_test(void)
{
    uint8_t buffer_out[2048] = {0};
    uint32_t transactions_pass = 0, transactions_fail = 0;
    uint32_t start_time;
    int8_t res = COINES_SUCCESS;

    for (uint16_t i = 0; i < sizeof(buffer_out); i++)
    {
        buffer_out[i] = (uint8_t)i;
    }

    for (uint32_t n = 32; (n <= sizeof(buffer_out)) && (res == COINES_SUCCESS); n = n * 2)
    {
        transactions_fail = 0;
        transactions_pass = 0;
        printf("Payload size of %u bytes \r\n", n);
        start_time = coines_get_millis();
        while (coines_get_millis() <= (start_time + 5000))
        {
            //printf("%u %u\n", transactions_pass, n);
            res = coines_echo_test(buffer_out, n);
            check(res);
            if (res)
            {
                transactions_fail++;
            }
            else
            {
                transactions_pass++;
            }
        }

        printf("Number of transactions in 5 seconds :: Pass: %u, Fail: %u. Throughput %u Bps\r\n", transactions_pass, transactions_fail, transactions_pass * n / 5);
    }
}