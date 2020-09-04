#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
/*
 *
 *
 void pinMode (int pin, int mode); //INPUT, OUTPUT
 void pullUpDnControl (int pin, int pud) ;//PUD_OFF PUD_UP PUD_DOWN
 void digitalWrite (int pin, int value) ;//HIGH  LOW 
 int digitalRead (int pin) ;
 void delayMicroseconds (unsigned int howLong)
 void delay (unsigned int howLong)
 wiringPiSetup (void) ;
 wiringPiSetupGpio (void) ;
 */
//蓝 GPIO.4 CS
//紫 GPIO.1 CLK
//灰 GPIO.2 DIN
#define DELAY_UNIT 2

#define DIN_PIN    1
#define LOAD_PIN   4
#define CLK_PIN    5

//addr define
#define NO_OP_ADDR      0x0000
#define DIG0_ADDR       0x0100
#define DIG1_ADDR       0x0200
#define DIG2_ADDR       0x0300
#define DIG3_ADDR       0x0400
#define DIG4_ADDR       0x0500
#define DIG5_ADDR       0x0600
#define DIG6_ADDR       0x0700
#define DIG7_ADDR       0x0800
#define DECODE_ADDR     0x0900
#define INTENSITY_ADDR  0x0A00
#define SCAN_LIMIT_ADDR 0x0B00
#define SHUTDOWN_ADDR   0x0C00
#define DIST_TEST_ADDR  0x0F00

//data define
//shutdown reg
#define SHUTDOWN_MODE       0x0000
#define NORMAL_MODE         0x0001

//decode reg
#define NO_DECODE          0x0000 //all digit will not be decode
#define DECODE_ALL         0x00FF //decode all digit as BCD

//scan limit reg
#define SCAN_ALL           0x0007

//Intensity reg
#define DEFAULT_INTENSITY  0x0003

void GPIO_Init() {
    wiringPiSetup () ;
    pinMode (DIN_PIN, OUTPUT);
    pinMode (LOAD_PIN, OUTPUT);
    pinMode (CLK_PIN, OUTPUT);
    
    digitalWrite (DIN_PIN, LOW);
    digitalWrite (LOAD_PIN, LOW);
    digitalWrite (CLK_PIN, LOW);
}

void GPIO_Exit() {
    pinMode (DIN_PIN, INPUT);
    pinMode (LOAD_PIN, INPUT);
    pinMode (CLK_PIN, INPUT);
}

void writeWords(const uint16_t* arr, int length) {
    digitalWrite(LOAD_PIN, LOW);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(DELAY_UNIT);

    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 16; j++) {
            if ((0x8000 >> j) & arr[i]) {
                digitalWrite(DIN_PIN, HIGH);
                //printf("1");
            }
            else {
                digitalWrite(DIN_PIN, LOW);
                //printf("0");
            }
            delayMicroseconds(DELAY_UNIT);
            digitalWrite(CLK_PIN, HIGH);
            delayMicroseconds(DELAY_UNIT*2);
            digitalWrite(CLK_PIN, LOW);
            delayMicroseconds(DELAY_UNIT);
        }
    }
    //printf("\n");

    digitalWrite(LOAD_PIN, HIGH);
    delayMicroseconds(DELAY_UNIT*2);
    digitalWrite(LOAD_PIN, LOW);
}

//chip_num 多片串联在一起 一起初始化
void initMAX7219(int chip_num) {
    #define INIT_INSTRUCTION_NUM 4
    const uint16_t init_data[INIT_INSTRUCTION_NUM] = {
        SHUTDOWN_ADDR | NORMAL_MODE,
        DECODE_ADDR | NO_DECODE,
        SCAN_LIMIT_ADDR | SCAN_ALL,
        INTENSITY_ADDR | DEFAULT_INTENSITY
        };
    
    uint16_t* config_buf = (uint16_t*)malloc(sizeof(uint16_t) * chip_num);

    for (int i = 0; i < INIT_INSTRUCTION_NUM; i++) {
        printf("%04x\n", init_data[i]);
        for (int j = 0; j < chip_num; j++) {
            config_buf[j] = init_data[i];
        }
        //writeWords(init_data + i, 1);
        writeWords(config_buf, chip_num);
        delayMicroseconds(DELAY_UNIT*4);
    }
    free(config_buf);
}

int main (int argc, char **argv)
{
    uint16_t* databuf; 
    int chipnum = 0;
    sscanf(argv[1], "%d", &chipnum);
    databuf = (uint16_t*)malloc(sizeof(uint16_t) * chipnum);
    printf("gpt chip num=%d\n", chipnum);
    GPIO_Init();
    initMAX7219(chipnum);
    for (;;)
    {
        for (int k =0; k < chipnum; k++) {
            scanf("%hx", databuf + k);
        }
        if (databuf[0] & 0x8000) {
            GPIO_Exit();
            break;
        }
        writeWords(databuf, chipnum);
        //printf("curren_value=%04x\n", arr[0]);
        //scanf("%hx", &a);
        //if (a & 0x8000) {
        //    GPIO_Exit();
        //    break;
        //}
        //arr[0] = arr[1];
        //arr[1] = a;
    }
    return 0;
}
