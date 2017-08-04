#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#define RS485_TX 1
#define RS485_RX 2

/* 
	RS232 -> RS485 -> RS232
	RS-485/232 Test program 

	control gpio on/off by using RS485
	Jeonghun, Lee 

*/ 
void RS485_TEST(int sel,int val) 
{
	int cnt=0, fd;
	struct termios settio;
	unsigned char buf[1];
	int ret=0;

	printf("RS485 START!! \n");	
	fd = open( "/dev/ttyS0", O_RDWR | O_NOCTTY ); // Blocking Mode 
	memset( &settio, 0, sizeof(settio) );	
	printf("RS485 OPEN OK!!! \n");	

//baudrate setting and bit, refer to, man  termtios , http://man7.org/linux/man-pages/man3/termios.3.html
	settio.c_cflag = B9600; //baudrate 
	settio.c_cflag |= CS8;  //8N1, 8bit , No parity, 1 Stop bit 
	settio.c_cflag |= CLOCAL; // Local Connection 
	settio.c_cflag |= CREAD;  // 
	settio.c_iflag = 0; 
	settio.c_oflag = 0;
	settio.c_lflag = 0;


	settio.c_cc[VTIME] = 10; /* Minimum number of characters for noncanonical read (MIN). for read buffer   */
	settio.c_cc[VMIN] = 1;   /* Timeout in deciseconds for noncanonical read (TIME). ,   for read timeout  */

	tcflush (fd, TCIFLUSH );
	tcsetattr(fd, TCSANOW, &settio ); 

	if(sel == RS485_TX){
		printf("RS485 TX TEST START!!!!! \n");				
		do{
			buf[0]=( cnt + 21  & 127) ;			
			ret = write( fd, buf,1 );  
			
			printf("write cnt=%d buf=%x \n",cnt,buf[0]);	
			if(ret <0) printf("error!! send  buf cnt=%d buf=%x \n",cnt,buf[0]);	

			cnt++;
		}while(cnt < val);	
		
		printf("SEND OK \n");
		
	}else if(sel == RS485_RX){
		printf("RS485 RX TEST START!!!!! \n");	
		do{
			ret = read( fd, buf,1 );  
			
			printf("read cnt=%d buf=%x \n",cnt,buf[0]);	
			if(ret <0)   printf("error!! recv  buf cnt=%d buf=%x \n",cnt,buf[0]);	
			cnt++;			
		}while(cnt < val);	
		
		printf("RECV OK \n");

	}
	close( fd); 
	return; 

}


int main(int argc, char* argv[]) 
{
	int val=0;

	printf("main start!! %s %s \n",argv[0],argv[1]);

	if(argc ==1){
		fputs("error!!, no argument!! \n", stderr);
		exit(1);
	}

	val = atoi(argv[2]); //rs485 argument 
	
	printf("main start!! %s %s %s (val=%d) \n",argv[0],argv[1],argv[2],val);
	
	if( val < 0){
		printf("error !!!!\n");
	}  	
		
	
	if( strcmp( argv[1],"TX") == 0){
		printf("TX START \n");
		RS485_TEST(RS485_TX,val);
	}else if( strcmp( argv[1],"tx") == 0){
		RS485_TEST(RS485_TX,val);
	}else if( strcmp( argv[1],"RX") == 0){
		RS485_TEST(RS485_RX,val);
	}else if( strcmp( argv[1],"rx") == 0){
		RS485_TEST(RS485_RX,val);
	}else
		printf("error!!!!! arguemnt!!!! \n");

	printf("program end!!! \n");
	

	return 0;


}

