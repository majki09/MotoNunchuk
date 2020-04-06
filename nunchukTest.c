#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <errno.h>
#include <ncurses.h>
#include <fcntl.h>
#include <string.h>

#define LCD_DEVICE "/dev/lcd"


void LCDsetup(void);
void nunchukSetup(int);
void LCDprintIntXY(int, char, char);

const int motorRangeLow = 980;
const int motorRangeHigh = 820;
const int NUNCHUCK_I2C_ID = 0x52;
const char DISPLAY_ON_SCREEN = 1;
const char DISPLAY_ON_LCD = 0;
int nunchukHandle;
int LCDhandle;

int main(void)
{
	char cText[20];
	
	initscr();		// Start curses mode

	printf("Testing the nunchuk through I2C\n");
	wiringPiSetup();
	nunchukSetup(NUNCHUCK_I2C_ID);
	LCDsetup();
		

	int bytes[6];
	int i;
	//int x_max=1, y_max=1;
	int x_max=237, y_max=227;
	//int accelX_max=0, accelY_max=0;
	int accelX_max=750, accelY_max=750, accelZ_max=750;
	//int x_min=255, y_min=255;
	int x_min=32, y_min=32;
	//int accelX_min=1024, accelY_min=1024;
	int accelX_min=300, accelY_min=300, accelZ_min=300;
	//int x_percent, y_percent;
	
	//WINDOW * labels_win = newwin (10, 20, 0, 0);
	//WINDOW * values_win = newwin (10, 20, 0, 8);
	WINDOW * joy_win = newwin(20, 40, 10, 1);
	WINDOW * accel_win = newwin(20, 40, 10, 45);
	//wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
	//box(labels_win, 0, 0);
	//box(values_win, 0, 0);
	box(joy_win, 0, 0);
	box(accel_win, 0, 0);
	
	//wmove(values_win, 1, 1);
	mvprintw(0, 0, "\tVAL\tMIN\tMAX\tPERCENT");
	
	refresh();
	//wrefresh(labels_win);
	//wrefresh(joy_win);
	
	while(1)
	{
		for (i=0; i<=5; i++)
		{
			bytes[i] = wiringPiI2CRead(nunchukHandle);
			delay(10); //needed ?
		}

		//bytes[5]=0;
		wiringPiI2CWrite(nunchukHandle, 0x00);
		//delayMicroseconds(500);
		//delay(50);

	
		int joyX = bytes[0];
		int joyY = bytes[1];
		int accelX = (bytes[2] << 2) | ((bytes[5] >> 2) & 0x03);
		int accelY = (bytes[3] << 2) | ((bytes[5] >> 4) & 0x03);
		int accelZ = (bytes[4] << 2) | ((bytes[5] >> 6) & 0x03);
		int c = (bytes[5] & 0x02) >> 1;
		int z = bytes[5] & 0x01;		
	
		/* OLD way
		int joyX = bytes[0];
		int joyY = bytes[1];
		int accelZ = (bytes[2] << 2) | ((bytes[5] & 0xc0) >> 6);
		int accelY = (bytes[3] << 2) | ((bytes[5] & 0x30) >> 4);
		int accelX = (bytes[4] << 2) | ((bytes[5] & 0x0c) >> 2);
		int c = (bytes[5] & 0x02) >> 1;
		int z = bytes[5] & 0x01;
		//char z = bytes[5];
		*/
	
	
		//if(joyX > x_max) x_max = joyX;
		//if(joyX < x_min) x_min = joyX;
		
		//if(joyY > y_max) y_max = joyY;
		//if(joyY < x_min) y_min = joyY;
		
		int joyX_percent = 100 - ((x_max - joyX)*100) / (x_max - x_min);
		int joyY_percent = 100 - ((y_max - joyY)*100) / (y_max - y_min);
		
		int accelX_percent = 100 - ((accelX_max - accelX)*100) / (accelX_max - accelX_min);
		int accelY_percent = 100 - ((accelY_max - accelY)*100) / (accelY_max - accelY_min);
		int accelZ_percent = 100 - ((accelZ_max - accelZ)*100) / (accelZ_max - accelZ_min);
		
		char znak;
		if (c==1) znak = 'o';
		else znak = ' ';
		
		mvprintw(1, 0, "joyX\t%d\t%d\t%d\t%d  ", joyX, x_min, x_max, joyX_percent);
		mvprintw(2, 0, "joyY\t%d\t%d\t%d\t%d  ", joyY, y_min, y_max, joyY_percent);
		mvprintw(3, 0, "ACC_X\t%d\t%d\t%d\t%d  ", accelX, accelX_min, accelX_max, accelX_percent);
		mvprintw(4, 0, "ACC_Y\t%d\t%d\t%d\t%d  ", accelY, accelY_min, accelY_max, accelY_percent);
		mvprintw(5, 0, "ACC_Z\t%d\t%d\t%d\t%d  ", accelZ, accelZ_min, accelZ_max, accelZ_percent);
		mvprintw(6, 0, "C\t%d", c);
		mvprintw(7, 0, "Z\t%d", z);
		//mvprintw(1, 0, "joyY= %d  \tjoyY_max= %d  \tjoyY_min= %d  \ty%= %d  \tC= %d   ", joyY, y_max, y_min, joyY_percent, c);
		//mvprintw(2, 0, "accelX= %d  \taccelX_max= %d  \taccelX_min= %d  \taccelX%= %d   ", accelX, accelX_max, accelX_min, accelX_percent);
		//mvprintw(3, 0, "accelY= %d  \taccelY_max= %d  \taccelY_min= %d  \taccelY%= %d   ", accelY, accelY_max, accelY_min, accelY_percent);
		//mvprintw(0, 18, "%d", joyX);
		
		//mvprintw(0, 10, "[
		//mvprintw(2, 0, "[                                                  ]");
		//mvprintw(3, 0, "[                                                  ]");
		//mvprintw(2, ((50*x_percent)/100), "x");
		//mvprintw(3, ((50*y_percent)/100), "x");
		mvprintw(1, 30 + ((50*joyX_percent)/100), "  x  ");
		mvprintw(2, 30 + ((50*joyY_percent)/100), "  x  ");
		
		//mvprintw(40-((40*y_percent)/100), ((80*x_percent)/100), "x");
		mvwaddch(joy_win, 20-((20*joyY_percent)/100), ((40*joyX_percent)/100), 'x');
		mvwaddch(accel_win, 20-((20*accelY_percent)/100), ((40*accelX_percent)/100), znak);
		
		refresh();
		//wrefresh(values_win);
		wrefresh(joy_win);
		wrefresh(accel_win);
		//fflush(stdout);
		//printf("bytes: %x %x %x %x %x %x\n", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
			// printf("data: joyX=%x joyY=%x accelX=%x accelY=%x accelZ=%x c=%x z=%x\n", joyX, joyY, accelX, accelY, accelZ, c, z);
			
		
		if (DISPLAY_ON_LCD)
		{
		//write(dfd, (const void *)joyY_percent, 2);
			sprintf(cText, "%d", joyY_percent);
			write (LCDhandle, "\e[2JjoyY_percent: ", 17);
			write (LCDhandle, cText, sizeof(joyY_percent)-2);
		//write (dfd, "\e[Ly1x3;X",9); 
		
		
		write (LCDhandle, "\e[2JX:    Y:", 12);		// clear LCD;
		
		LCDprintIntXY(joyX_percent, 2, 0);
		LCDprintIntXY(joyY_percent, 8, 0);
		
		LCDprintIntXY(0, ((20 * joyX_percent) / 100), 1);
		LCDprintIntXY(1, ((20 * joyY_percent) / 100), 2);
		
		
		int tempX = (20 * joyY_percent) / 100;
		char result[20];
		sprintf(cText, "%d", tempX);
		strcpy(result, "\e[Ly2x0;");
		strcat(result, cText);
		write (LCDhandle, result, 10);
		strcpy(result, "\e[Ly1x");
		strcat(result, cText);
		strcat(result, ";X");
			write (LCDhandle, result, 10);
		//mvprintw(2, 30 + ((50*joyY_percent)/100), "  x  ");
		//write (dfd, "\n", 1);
		//sprintf(cText, "%d", sizeof(joyY_percent));
		//write (dfd, cText, 5);
		
		LCDprintIntXY(24, 3, 3);
		
		
		LCDprintIntXY(((20 * joyY_percent) / 100), 0, 3);
		}
	}
	
	endwin();		// End curses mode
	
	close(LCDhandle);
	close(nunchukHandle);
	
	return 0;
}

void LCDsetup(void)
{
    if (DISPLAY_ON_LCD)
	{
		LCDhandle = open(LCD_DEVICE, O_WRONLY);
		if (LCDhandle < 0)
		{
			printf("LCD not detected");
			mvprintw(8, 0, "LCD NOT OK");
		}
		else
		{
		//k = write(dfd,"Hello\n", 6);
			//printf("write=%d\n", write(dfd,"LCD OK\n", 6));
			write(LCDhandle, "NUNCHUK TEST", 12);
			mvprintw(8, 0, "LCD OK    ");
		}
	
		mvprintw(8, 35, "LCDhandle=%d", LCDhandle);
	}
}

void nunchukSetup(int deviceNo)
{
    nunchukHandle = wiringPiI2CSetup(deviceNo);
	
	if (nunchukHandle < 0)
	{
		printf("Error setting up I2C: %d\n", errno);
		mvprintw(8, 15, "I2C NOT OK");
		// exit(errno);
	}
	else
	{	// nunchuk device init
		wiringPiI2CWriteReg8(nunchukHandle, 0xF0, 0x55);
		delayMicroseconds(500);
		wiringPiI2CWriteReg8(nunchukHandle, 0xFB, 0x00);
		delayMicroseconds(500);

		delay(500);
		mvprintw(8, 15, "I2C OK    ");
	}
	
	mvprintw(8, 25, "nunchukHandle=%d", nunchukHandle);
}

void LCDprintIntXY(int value, char x, char y)
{
    if (DISPLAY_ON_LCD)
	{
		char result[20];
		char cValue[5];
		char cX[2];
		char cY[2];
		
		if (value < 10)
			sprintf(cValue, "0%d", value);
		else
			sprintf(cValue, "%d", value);
			
		sprintf(cX, "%d", x);
		sprintf(cY, "%d", y);
		strcpy(result, "\e[Ly");
		strcat(result, cY);
		strcat(result, "x");
		strcat(result, cX);
		strcat(result, ";");
		strcat(result, cValue);
		
		write (LCDhandle, result, strlen(result));
	}
}
