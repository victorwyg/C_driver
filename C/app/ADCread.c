#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define SENSOR_FLOAT_DATA_GET(ret, index, str, member)\
    ret = file_data_read(file_path[index], str);\
    dev->member = atof(str);
#define SENSOR_INT_DATA_GET(ret,index,str,member)\
    ret = file_data_read(file_path[index],str);\
    dev->member = atoi(str);

static char *file_path[] = {
    "/sys/bus/iio/devices/iio:device0/in_voltage_scale",
    "/sys/bus/iio/devices/iio:device0/in_voltage2_raw",
    "/sys/bus/iio/devices/iio:device0/in_voltage3_raw",
    "/sys/bus/iio/devices/iio:device0/in_voltage4_raw",
    "/sys/bus/iio/devices/iio:device0/in_voltage5_raw",
    "/sys/bus/iio/devices/iio:device0/in_voltage6_raw",
    "/sys/bus/iio/devices/iio:device0/in_voltage7_raw",
    "/sys/bus/iio/devices/iio:device0/in_voltage8_raw",
    "/sys/bus/iio/devices/iio:device0/in_voltage9_raw",
};

enum path_index {
    IN_VOLTAGE_SCALE = 0,
    IN_VOLTAGE_RAW1,
    IN_VOLTAGE_RAW2,
    IN_VOLTAGE_RAW3,
    IN_VOLTAGE_RAW4,
    IN_VOLTAGE_RAW5,
    IN_VOLTAGE_RAW6,
    IN_VOLTAGE_RAW7,
    IN_VOLTAGE_RAW8,
};

struct adc_dev{
    int raw1,raw2,raw3,raw4,raw5,raw6,raw7,raw8;
    float scale;
    float act1,act2,act3,act4,act5,act6,act7,act8;
};

struct adc_dev AirP;
static int file_data_read(char *filename,char *str){
    int ret = 0;
    FILE *data_stream;

    data_stream = fopen(filename,"r");
    if(data_stream == NULL){
        printf("can`t open file %s\r\n",filename);
        return -1;
    }
    ret = fscanf(data_stream,"%s",str);
    if(!ret){
        printf("file read erro %s\r\n",filename);
    }else if(ret == EOF){
        fseek(data_stream,0,SEEK_SET);
    }
    fclose(data_stream);
    return 0;
}
static int adc_read(struct adc_dev *dev){
    int ret = 0;
    char str[50];
    SENSOR_FLOAT_DATA_GET(ret,IN_VOLTAGE_SCALE,str,scale);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW1,str,raw1);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW2,str,raw2);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW3,str,raw3);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW4,str,raw4);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW5,str,raw5);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW6,str,raw6);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW7,str,raw7);
    SENSOR_INT_DATA_GET(ret,IN_VOLTAGE_RAW8,str,raw8);

    dev->act1 = (dev->scale * dev->raw1)/1000.f*16-3.12;
    dev->act2 = (dev->scale * dev->raw2)/1000.f*16-3.2;
    dev->act3 = (dev->scale * dev->raw3)/1000.f*16-3.2;
    dev->act4 = (dev->scale * dev->raw4)/1000.f*16-3.2;
    dev->act5 = (dev->scale * dev->raw5)/1000.f*16-3.2;
    dev->act6 = (dev->scale * dev->raw6)/1000.f*16-3.12;
    dev->act7 = (dev->scale * dev->raw7)/1000.f*16-3.2;
    dev->act8 = (dev->scale * dev->raw8)/1000.f*16-3.2;

    return ret;

}

int main(int argc,char *argv[])
{
    int ret =0;
    if(argc !=1){
        printf("Error Usage\r\n");
        return -1;
    }
   
        ret = adc_read(&AirP);
        if(ret == 0){
            printf("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",AirP.act1,AirP.act2,AirP.act3,AirP.act4,AirP.act5,AirP.act6,AirP.act7,AirP.act8);
        }
    return 0;
}