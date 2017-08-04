#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>


//#define STDIN_INPUT_TEST // Not work properly so now not used. 
#define WRT_CMD_TST
#define WRT_SLT_CMD  


#define CNT_1ST		(4096-1) // Command Timing Control 
#define CNT_2ND		(512-1)  // Command Timing Control 

/*
*  
*/

#define SERIAL_TTY 	"/dev/ttyACM0"  // USB to Serial 

#define FALSE		0
#define TRUE		1

#define MAX_RX_BUF	16
#define MAX_TX_BUF	16

#define MAX_CMD		16

#define STDIN		0

/* 
	Simple 232 Test program 
*/ 


char cmd[][MAX_CMD]={  // My MCU Paser TEST 
	"MT+SRT",
	"MT+GTSTS",	
	"MT+STGRL",	
	"MT+STRPT",	
	"MT+RST",
};


void RS232_TEST(void) 
{
	int fd;
	struct termios settio;
	char rxbuf[MAX_RX_BUF];
	char txbuf[MAX_TX_BUF];
	int ret=0;
	volatile int stop=FALSE;
	unsigned int cnt1=0,cnt2=0;
	unsigned int sel=0,set=0;
	int flags;
	

	printf("RS232 START!! \n");	
	fd = open( SERIAL_TTY, O_RDWR | O_NOCTTY | O_NONBLOCK ); // Non-Blocking Mode 
//	fd = open( SERIAL_TTY, O_RDWR | O_NOCTTY  ); // Blocking Mode 
	
	memset( &settio, 0, sizeof(settio) );	
	printf("RS232 OPEN OK!!! %s , %d \n",SERIAL_TTY, fd);	

	settio.c_cflag = B115200; //baudrate 
	settio.c_cflag |= CS8;  //8N1, 8bit , No parity, 1 Stop bit 
	settio.c_cflag |= CLOCAL; // Local Connection 
	settio.c_cflag |= CREAD;  // 

	settio.c_iflag = IGNPAR | ICRNL; //check Parity , CR->NL(New Line)
	settio.c_oflag = 0;
	settio.c_lflag = 0;

	settio.c_cc[VTIME] = 5 ; /* Minimum number of characters for noncanonical read (MIN). for read buffer   */
	settio.c_cc[VMIN] =  1;  /* Timeout in deciseconds for noncanonical read (TIME). ,   for read timeout  */

	tcflush (fd, TCIFLUSH );
	tcsetattr(fd, TCSANOW, &settio ); 

//        cfsetospeed(&settio,B115200);            // 115200 baud
//        cfsetispeed(&settio,B115200);            // 115200 baud

	cnt1=0;	
	cnt2=0;
	sel=0;

#if 0 //defined(STDIN_INPUT_TEST)
	flags = fcntl(STDIN, F_GETFL, 0);//STDIN
	fcntl(STDIN,F_SETFL, flags | O_NONBLOCK);
#endif
	do{
		ret = read(fd, rxbuf ,1 );	
		if(ret > 0) printf("%c",rxbuf[0]);
		
#if defined(STDIN_INPUT_TEST)

		ret = read(STDIN, &rxbuf[1],1); //STDIN 

		if(ret > 0) { 

			if(rxbuf[1] >= '0' && rxbuf[1] <= '9'){
				sel = rxbuf[1]-'0';
				set = 1;
			}
			printf("[%c],sel=%d",rxbuf[1],sel);             

		}
#endif
   	  
#if defined(WRT_CMD_TST)		
		if( (cnt1 & CNT_1ST ) == 0 ){ 		
			
			if( (cnt2 & CNT_2ND ) == 0 ){ 		
				int i;

#if defined(STDIN_INPUT_TEST)
				if(set ==1){

					if(sel >=0 && sel <= 9){
						memset(txbuf,0,MAX_TX_BUF);
						strncpy(txbuf,cmd[sel],sizeof(cmd[sel]));		
						//ret = write(fd, txbuf, MAX_TX_BUF );
						ret = write(fd, txbuf, MAX_TX_BUF );
						if(ret > 0)	printf("\n%s(%d):",txbuf,ret);			
					}

					set=0;
		
				}

#else //defined(STDIN_INPUT_TEST)
				strncpy(txbuf,cmd[sel],sizeof(cmd[sel]) );		
	#if defined(WRT_CMD_CHAR)
				for(i=0;i<MAX_TX_BUF;i++)
					ret = write(fd, &txbuf[i], 1 );  			
	#else
				//ret = write(fd, txbuf, MAX_TX_BUF );  			
				ret = write(fd, txbuf, strlen(txbuf)+1 );  			
	#endif
				if(ret > 0)	printf("\n%s(%d):",txbuf,ret);			

#endif//STDIN_INPUT_TEST

#if !defined(STDIN_INPUT_TEST)
				sel = (sel +1) % 4;	
#endif

			}
			cnt2++;
		}

#endif
		cnt1++;

	}while(stop ==FALSE);			
		
	close(fd); 
	return; 

}


int main(int argc, char* argv[]) 
{
	int val=0;

	printf("PC Main TEST Program  start!! \n");					
	RS232_TEST();
	printf("program end!!! \n");
	return 0;
}

