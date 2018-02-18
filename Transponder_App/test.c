#include    <stdio.h>
#include	<stdlib.h>
#include	<string.h>

int	channel						= (0);

#define	MYLAPS_CLOCK_LOW_HIGH		( 80 / 5 / 2 )						// 80 Mhz base, then 5 Mhz for the carrier with 50% dutty
#define	MYLAPS_CLOCK_PHASE_CHG		( MYLAPS_CLOCK_LOW_HIGH * 2 )		// 80 Mhz base, then 5 Mhz for the carrier for phase change

#define TELEGRAM_SIZE   			( 12 * 8 * 4 )						/*!< Telegram has 12 bytes, so 96 bits */
#define RMT_TX_DATA_NUM  			1									/*!< NEC tx test data number */

#define msg_len     12
#define msg_count   8
#define TRANS_COUNT	1

#define	true	( 1 == 1 )
#define false	( 1 == 0 )


typedef struct {
    union {
        struct {
            unsigned int duration0 :15;
            unsigned int level0 :1;
            unsigned int duration1 :15;
            unsigned int level1 :1;
        };
        unsigned int val;
    };
} rmt_item32_t;

// 2351957
static unsigned char   radio_msg [msg_count*TRANS_COUNT][msg_len]   = 
    {
        {   0XF9, 0X16, 0XE1, 0XCB, 0X12, 0X1C, 0XC9, 0XD6, 0XC3, 0XE0, 0XFF, 0X0F  },		// Transponder ID Telegram
        {   0XF9, 0X16, 0XDA, 0XE7, 0X94, 0X77, 0XE9, 0X3C, 0X91, 0XD7, 0XC3, 0XCC  },		// Status Telegram
        {   0XF9, 0X16, 0XEC, 0X50, 0X55, 0X92, 0XE2, 0X23, 0X61, 0XD4, 0XF0, 0X0C  },
        {   0XF9, 0X16, 0X36, 0X58, 0X15, 0X1B, 0XC8, 0XC3, 0X62, 0X14, 0X3C, 0X00  },
        {   0XF9, 0X16, 0X0E, 0X29, 0XBA, 0XE0, 0X3E, 0XE3, 0X62, 0XDB, 0XC0, 0XC3  },
        {   0XF9, 0X16, 0X36, 0X55, 0X57, 0X09, 0XFB, 0X3F, 0X91, 0X27, 0X00, 0XF0  },
        {   0XF9, 0X16, 0X0E, 0XFE, 0XF0, 0X8A, 0X22, 0X3C, 0X52, 0X1B, 0X3F, 0XF3  },
        {   0XF9, 0X16, 0XD7, 0XA8, 0X10, 0X77, 0XD1, 0X23, 0XA2, 0XD7, 0XC3, 0X3C  }
	};
	
//******************************************************************************************************
	
#define	OUT_LEN		13					//v0.01
#define POLY		0x1000EEC20F	//v0.01
#define BIT_NUMBER	50

unsigned char * Mylaps_Encoder( unsigned long inp,	unsigned int gInt, unsigned char  lastByte )
{  // Array of environment variable strings

	unsigned long long 	fflop	= 0,
						temp	= 0,
						temp1;
						
	unsigned char 		*out	= (unsigned char *)malloc( OUT_LEN );

	unsigned int i, j, k;
	
//	lastByte= 0;
//	gInt= 0;

	if( ! out )
		return out;
	
	memset( out, 0 ,OUT_LEN );

	inp|= ( unsigned long long )(lastByte<<24);		//add last byte to number input
	j=0;
	for(i= 0; i< 36 ; i++) 
	{
		temp<<=1;
		if(j< 3) 
		{														//this is not ghost bit
			temp|= ((inp& 0x04000000)>>26);			//add bit from number to stream
			inp<<=1;
		} 
		else 
		{																//this is ghost bit 
			temp|= (gInt& 0x0100)>> 8;					//add ghost bit to stream
			gInt<<=1;
		}
		if(j< 3) 
			j++;
		else 
			j= 0;
//		printf("%1d", (temp&0x01));
	}
//	printf("\r\nTEMPH: 0x%08x\r\n", temp>> 32);	//printf can not print 64bit at once
//	printf("TEMPL: 0x%08x\r\n", temp& 0xffffffff);
//	printf("\r\n");

	j=0;
	k=2;
	fflop|= (temp&0x01);													//add in stream to ff 
	temp>>=1;																//shift in stream	
	for(i= 0; i< BIT_NUMBER; i++)	
	{
		fflop	<<=	1;														//shift flipflops
		fflop	|= 	(temp&0x01);											//add in stream to ff 
		temp1	= 	fflop& POLY;											//mask ff with polynomial
		temp1	^= 	temp1 >> 1;												//calculate parity (number of 1's in stream)
		temp1	^= 	temp1 >> 2;
		temp1	= 	(temp1 & 0x1111111111111111UL) * 0x1111111111111111UL;
		temp1	= 	(temp1 >> 60) & 0x01;

		out[k]	<<=	1;														//shift output
		out[k]	|= 	temp1;													//add to output
		out[k]	<<=	1;														//shift output		
		out[k]	|= 	(temp1^ ((fflop>>1)&0x01));								//add to output

		if(j< 3) 
			j++;
		else 
		{
			//printf("OUT[%02d]= 0x%02x\r\n",k, out[k]);
			j	=	0;
			k++;
		}
		/*
		printf("IN: %01d= ", temp&0x01);
		printf("%01d ", temp1);
		printf("%01d ", temp1^ ((fflop>>1)&0x01) );
		printf("FF= 0x%08x\r\n", fflop&0xff);
		*/
		temp	>>=	1;															//shift in stream	
	}


	out[0]	= 	0xf9;
	out[1]	= 	0x16;
	printf("Output: ");
	printf("stream= {");
	for(i= 0; i< 11; i++)
		printf("0x%02X, ", out[i]);
	printf("0x%02X };\n", out[i]);
	return out;
}	
//******************************************************************************************************
static int MyLaps_CreateTelegram(int channel, rmt_item32_t* item, int item_num, unsigned char *msg)
{
    int 	i 			= 0,
			j 			= 0;
	int		last_bit	= ((0b010000000) & (msg[0])) ? true : false;				// initialise the n-1 bits
	int		phase		= false;
	
    if(item_num < TELEGRAM_SIZE)				// Check if we have space....
	{
        return -1;
    }

    for(j = 0; j < TELEGRAM_SIZE / 4  ; j++)
	{
		int		current_bit	= ((0b010000000 >> ( j % 8 )) & msg[ j / 8 ])? true : false;

		for( int b=0;b<8;b++)
		{
			if( current_bit == last_bit ) 	// test if previsous bit is the same 
			{ 																				// the and four cycles
				if( i % 2 )
				{
					item[i/2].duration0	=	MYLAPS_CLOCK_LOW_HIGH;
					item[i/2].level0	=	0;
				}
				else
				{
					item[i/2].duration1	=	MYLAPS_CLOCK_LOW_HIGH;
					item[i/2].level1	=	1;
				}
				i++;
			}
			else																			// we have a phase change...
			{
				phase 		= ! phase;														// invert the phase
				last_bit 	=  current_bit;	// copy the last_bit....
			int	l =	i-1;
				if( i > 0 )																	// only to be sure we are not on start....
				{
					if( l % 2 )
					{
						item[l/2].duration0	=	MYLAPS_CLOCK_PHASE_CHG;					
					}
					else
					{
						item[l/2].duration1	=	MYLAPS_CLOCK_PHASE_CHG;
					}
				}
				if( b )
					printf("Something wrong at: %d,%d,%d - %d;%d ** ", i,b,j,current_bit,last_bit);
			}
		}
		last_bit 	= current_bit ;			// copy the last_bit....
    }
    return i;
}

void rmt_write_items(int channel, rmt_item32_t *item, int item_num, int flag)
{
	printf("\n");
	
	int		t = 0;
	
	for( int i = 0; i<item_num; i ++ )
	{
	int 	d,l;
	
		if( i % 2 )
		{
			d = item[i/2].duration0;
			l = item[i/2].level0;
		}
		else
		{
			d = item[i/2].duration1;
			l = item[i/2].level1;
		}
		
		if( d == 0 )
		{
			printf("\nEnd at %d ( %d,%d)",i,d,l);
			break;
		}
		t += d;
		
		if( d != MYLAPS_CLOCK_LOW_HIGH )
		{
			printf("%s", l ? "X X":"_ _" );
		}
		else
		{
			printf("%s", l ? "X":"_" );
		}
		if( ( t % (  MYLAPS_CLOCK_LOW_HIGH * 4 * 2 ) ) == 0 )
			printf(" ");
	}
	
	printf("\n");
}
	
int main( int argc,char **argv)
{
		size_t 			size 		= (sizeof(rmt_item32_t) * TELEGRAM_SIZE );
        //each item represent a cycle of waveform.
		rmt_item32_t* 	item 		= (rmt_item32_t*) malloc(size);
		int 			item_num 	= TELEGRAM_SIZE ;
		
		int 			i,
						offset 		= 0;
						
		memset((void*) item, 0, size);
		
		printf("Creating radio :\n");
        i = MyLaps_CreateTelegram(channel, item , item_num,&radio_msg[0][0]);
		printf(" inserted %d items \n",i);
		rmt_write_items(channel, item, item_num, true);

		free( Mylaps_Encoder( 2351957, 0, 0 ) );
		free( Mylaps_Encoder( 2688158, 0, 0 ) );
		free( Mylaps_Encoder( 3073479, 0, 0 ) );
		free( Mylaps_Encoder( 4572215, 0, 0 ) );
		free( Mylaps_Encoder( 4748687, 0, 0 ) );
		free( Mylaps_Encoder( 4961721, 0, 0 ) );
		free( Mylaps_Encoder( 5564690, 0, 0 ) );
		free( Mylaps_Encoder( 6000513, 0, 0 ) );
		free( Mylaps_Encoder( 6091953, 0, 0 ) );
		free( Mylaps_Encoder( 6099608, 0, 0 ) );
		free( Mylaps_Encoder( 7531106, 0, 0 ) );
		free( Mylaps_Encoder( 3456789, 0, 0 ) );
		

	return 0;
}
