#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/kdev_t.h>

#define LED_FILE_NAME   "/dev/dot_driver"

int main(int argc, char **argv)
{
        int     fd;
        char    data;

        fd = open(LED_FILE_NAME, O_RDWR);
        if (fd < 0)
        {
                fprintf(stderr, "Can't open %s\n", LED_FILE_NAME);
                return -1;
        }

        while (1)
        {
                data = 1;
                write(fd, &data, sizeof(char));
                sleep(1);
        }
        close(fd);
        return 0;
}
