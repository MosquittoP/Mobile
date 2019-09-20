#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>
#include <linux/poll.h>
#include <signal.h>

#define KEY_FILE_NAME   "/dev/key_driver"

int main()
{
        int             key_fd;
        char            led_onoff = 0;
        char            key;
        struct pollfd   events[2];
        int             retval;

        key_fd = open(KEY_FILE_NAME, O_RDWR | O_NONBLOCK);
        if (key_fd < 0)
        {
                fprintf(stderr, "Can't open %s\n", KEY_FILE_NAME);
                return -1;
        }

        while (1)
        {
                events[0].fd            = key_fd;
                events[0].events        = POLLIN;       // waiting read

                retval = poll(events, 1, 1000);         // event waiting
                if (retval < 0)
                {
                        fprintf(stderr, "Poll error\n");
                        exit(0);
                }

                if (events[0].revents & POLLIN)
                {
                        //puts("[APP] Wakeup_Poll_Event!!\n");
                        if (key % 2 == 0)
                        {
                        led_onoff = 0;
                        write(key_fd, &led_onoff, 1);
                        read(key_fd, &key, 1);
                        printf("[APP] KEY: %d\n", key);
                        }
                }
        }

        led_onoff = 0;
        write(key_fd, &led_onoff, 1);

        close(key_fd);
        return 0;
}
