#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>
#include <linux/poll.h>
#include <signal.h>

#define MOTER_FILE_NAME "/dev/moter_driver"

int main()
{
        int             moter_fd;
        char            moter_onoff = 0;
        char            moter;
        struct pollfd   events[2];
        int             retval;

        moter_fd = open(MOTER_FILE_NAME, O_RDWR | O_NONBLOCK);
        if (moter_fd < 0)
        {
                fprintf(stderr, "Can't open %s\n", MOTER_FILE_NAME);
                return -1;
        }

        while (1)
        {
                events[0].fd            = moter_fd;
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
                        if (moter % 2 == 0)
                        {
                        moter_onoff = 0;
                        write(moter_fd, &moter_onoff, 1);
                        read(moter_fd, &moter, 1);
                        printf("[APP] moter: %d\n", moter);
                        }
                }
        }

        moter_onoff = 0;
        write(moter_fd, &moter_onoff, 1);

        close(moter_fd);
        return 0;
}
