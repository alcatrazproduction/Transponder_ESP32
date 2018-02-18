/* 
 * Transponder emulation for MyLaps
 * 
 * From a developpement from Cano
 * 
 * Telegram have 80 bits to send
 * Modulation is B-PSK
 * 
 * Data rate is 5Mhz
 * 
 * Why try with master clock 80 Mhz, period is 12.5 ns
 * 
 * with a pixel clock of 80 Mhz, to change the phase, we need 4 ticks, and clock is 8 ticks
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_err.h"
#include "esp_log.h"

#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_reg.h"
#include "esp_attr.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"

static const char* MYLAPS_TAG = "MyLaps";

//CHOOSE SELF TEST OR NORMAL TEST

#define RMT_TX_CARRIER_TEST   0

#define STORAGE_NAMESPACE 	"MyLaps"
#define STORAGE_RADIO_MSG	"radio_msg"

/******************************************************/
/*****                SELF TEST:                  *****/
/*Connect RMT_TX_GPIO_NUM with RMT_RX_GPIO_NUM        */
/*TX task will send NEC data with carrier disabled    */
/*RX task will print NEC data it receives.            */
/******************************************************/

#if RMT_TX_CARRIER_TEST

#define RMT_TX_CARRIER_EN    true   /*!< Disable carrier for self test mode  */

#else

#define RMT_TX_CARRIER_EN    false   /*!< Enable carrier for IR transmitter test with IR led */

#endif

#define RMT_TX_CHANNEL    			0									/*!< RMT channel for transmitter */
#define RMT_TX_GPIO_NUM  			26									/*!< GPIO number for transmitter signal */
#define _RMT_TX_GPIO_NUM  			27									/*!< GPIO number for transmitter signal (Inverted)*/

#define RMT_CLK_DIV      			1									/*!< RMT counter clock divider */

#define	MYLAPS_CLOCK_LOW_HIGH		( 80 / 5 / 2 )						// 80 Mhz base, then 5 Mhz for the carrier with 50% dutty
#define	MYLAPS_CLOCK_PHASE_CHG		( MYLAPS_CLOCK_LOW_HIGH * 2 )		// 80 Mhz base, then 5 Mhz for the carrier for phase change

#ifndef true 
#	define	true	( 1 == 1 )
#endif

#ifndef false 
#	define false	( 1 == 0 )
#endif


#define TELEGRAM_SIZE   			( 12 * 8 * 4)						/*!< Telegram has 12 bytes, so 96 bits */
#define RMT_TX_DATA_NUM  			1									/*!< NEC tx test data number */

#define msg_len     12
#define msg_count   8
#define TRANS_COUNT	1

// 2351957
static unsigned char   radio_msg_default [msg_count*TRANS_COUNT][msg_len]   = 
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
	
static unsigned char * 			radio_msg 		= NULL;
static size_t					radio_msg_size	= 0;

static volatile size_t 			size 			= 2048;												// 2k block
static volatile rmt_item16_t* 	rmt_current 	= NULL;												// pointer to wave data
static volatile rmt_item16_t*	rmt_delay		= NULL;												// last block, try to use it as delay time


wifi_config_t 			staConfig 		= 
	{
		.sta = 
		{
			.ssid="<access point name>",
			.password="<password>",
			.bssid_set=false
		}
	};
	
wifi_config_t 			apConfig 		= 
	{
		.ap = 
		{
			.ssid="Transponder_2351975",
			.ssid_len=0,
			.password="2351957",
			.channel=1,
			.authmode=WIFI_AUTH_OPEN,
			.ssid_hidden=0,
			.max_connection=4,
			.beacon_interval=100
		}
	};
/* Wifi event handler
 * 
 */
static 	EventGroupHandle_t 	event_group;
const 	int 				STA_CONNECTED_BIT 		= BIT0;
const 	int 				STA_DISCONNECTED_BIT 	= BIT1;
const 	int 				WIFI_CONNECTED_BIT 		= BIT0;


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) 
	{		
		case SYSTEM_EVENT_AP_STACONNECTED:
			xEventGroupSetBits(event_group, STA_CONNECTED_BIT);
			break;

		case SYSTEM_EVENT_AP_STADISCONNECTED:
			xEventGroupSetBits(event_group, STA_DISCONNECTED_BIT);
			break;		
		
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			break;
		
		case SYSTEM_EVENT_STA_GOT_IP:
			xEventGroupSetBits(event_group, WIFI_CONNECTED_BIT);
			break;
		
		case SYSTEM_EVENT_STA_DISCONNECTED:
			xEventGroupClearBits(event_group, WIFI_CONNECTED_BIT);
			break;
		
		default:
			break;    
	}
   
	return ESP_OK;
}

	/* Save new run time value in NVS
   by first reading a table of previously saved values
   and then adding the new value at the end of the table.
   Return an error if anything goes wrong
   during this process.
 */
esp_err_t SaveTransponderData(unsigned char *buffer, size_t size)
{
	nvs_handle 	my_handle;
	esp_err_t 	err;

	// Open
	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK) 
		return err;

	// Read the size of memory space required for blob
	// Write value including previously saved blob if available
	
	err = nvs_set_blob(my_handle, STORAGE_RADIO_MSG, buffer, size);

	if (err != ESP_OK) 
		return err;

	// Commit
	err = nvs_commit(my_handle);
	if (err != ESP_OK) 
		return err;

	// Close
	nvs_close(my_handle);
	return ESP_OK;
}

	/* Save new run time value in NVS
   by first reading a table of previously saved values
   and then adding the new value at the end of the table.
   Return an error if anything goes wrong
   during this process.
 */
esp_err_t ReadTransponderData(unsigned char **buffer, size_t *size)
{
	nvs_handle 	my_handle;
	esp_err_t 	err;

	// Open
	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK) 
		return err;

	// Read the size of memory space required for blob
	size_t 		required_size 	= 0;  // value will default to 0, if not set yet in NVS
	err = nvs_get_blob(my_handle, STORAGE_RADIO_MSG, NULL, &required_size);
	if (err != ESP_OK ) 
		return err;

	// Read previously saved blob if available
	if( required_size == 0 )
		required_size = *size;
	if( required_size != *size )
	{
		*buffer = realloc( *buffer,required_size );
		*size 	= required_size;
	}
	
	if (required_size > 0) 
	{
		err = nvs_get_blob(my_handle, STORAGE_RADIO_MSG, *buffer, &required_size);
		if (err != ESP_OK) 
			return err;
	}

	// Close
	nvs_close(my_handle);
	return ESP_OK;
}

#define	OUT_LEN		13					//v0.01
#define POLY		0x1000EEC20F	//v0.01
#define BIT_NUMBER	50

/*
 * @brief Encode  Mylaps transponder.
 */

static unsigned char * Mylaps_Encoder( unsigned long inp,	unsigned int gInt, unsigned char  lastByte )
{
	unsigned long long 	fflop	= 0,
						temp	= 0,
						temp1;
						
	unsigned char 		*out	= (unsigned char *)malloc( OUT_LEN );

	unsigned int i, j, k;

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
	}

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
			j	=	0;
			k++;
		}
		temp	>>=	1;															//shift in stream	
	}


	out[0]	= 	0xf9;
	out[1]	= 	0x16;
	return out;
}	

/*
 * @brief Build Mylaps transponder waveform.
 */

	static int MyLaps_CreateTelegram(int channel, volatile rmt_item16_t* item, int item_num, unsigned char *msg)
{
    int 	i 			= 0,
			j 			= 0;
	int		last_bit	= ((0b010000000 >> ( 0 % 8 )) & msg[ 0 / 8 ])? true : false;				// initialise the n-1 bits
		
    if(item_num < TELEGRAM_SIZE)				// Check if we have space....
	{
        return -1;
    }

    for(j = 0; j < TELEGRAM_SIZE / 4  ; j++)
	{
		int		current_bit	= ((0b010000000 >> ( j % 8 )) & msg[ j / 8 ])? true : false;
		int		duration	= MYLAPS_CLOCK_LOW_HIGH;
		
		if( current_bit != last_bit ) 	// test if previsous bit is the same
			duration	= MYLAPS_CLOCK_PHASE_CHG;
		
		for( int b=0;b<8;b++)
		{
//			if( current_bit == last_bit ) 	// test if previsous bit is the same 
			{ 																				// the and four cycles
//				if( i % 2 )
//				{
//					item[i/2].duration0	=	duration;
//					item[i/2].level0	=	0;
//				}
//				else
//				{
//					item[i/2].duration1	=	duration;
//					item[i/2].level1	=	1;
//				}
				item[i].duration	=	duration;
				item[i].level		=	i % 2? 0 : 1;
				i++;
				duration	= MYLAPS_CLOCK_LOW_HIGH;
			}
		}
		last_bit 	= current_bit ;			// copy the last_bit....
    }
	rmt_delay = &item[i];
	rmt_delay->duration 	= 10 * MYLAPS_CLOCK_LOW_HIGH;
	rmt_delay->level		= 0;
	i++;
	item[i].duration		= 0;
	item[i].level			= 0;
   return i;
}


/*
 * @brief RMT transmitter initialization
 */
static void MyLaps_tx_init()
{
    rmt_config_t 		rmt_tx;
	
	nvs_flash_init();
   
    rmt_tx.channel 							= RMT_TX_CHANNEL;
    rmt_tx.gpio_num 						= RMT_TX_GPIO_NUM;
    rmt_tx.mem_block_num 					= 8;					// we need 96 bits and have 4 clocks cycles, so six block of 64
    rmt_tx.clk_div 							= RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en 				= true;
    rmt_tx.tx_config.carrier_duty_percent 	= 50;
    rmt_tx.tx_config.carrier_freq_hz 		= 20000000;				// if we use a carrier it will be 80Mhz 
    rmt_tx.tx_config.carrier_level 			= 1;
    rmt_tx.tx_config.carrier_en 			= RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level 			= 1;
    rmt_tx.tx_config.idle_output_en 		= true;
    rmt_tx.rmt_mode 						= RMT_MODE_TX;
	
    rmt_config(&rmt_tx);
	
    rmt_driver_install(rmt_tx.channel, 0, 0);
	
	// Set the direction of the first radio output ( not inverted)
	
	gpio_set_direction	( RMT_TX_GPIO_NUM, GPIO_MODE_OUTPUT);
	gpio_matrix_out		( RMT_TX_GPIO_NUM, RMT_SIG_OUT0_IDX, 0, 0);
	
	// Set the direction of the second radio output ( will be inverted)
	
	gpio_set_direction	(_RMT_TX_GPIO_NUM, GPIO_MODE_OUTPUT);
	gpio_matrix_out		(_RMT_TX_GPIO_NUM, RMT_SIG_OUT0_IDX, 1, 0);

	//PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[RMT_TX_GPIO_NUM], PIN_FUNC_GPIO);
	//gpio_pad_select_gpio(RMT_TX_GPIO_NUM);
	
	if( radio_msg == NULL )
	{
		if( ReadTransponderData( &radio_msg, &radio_msg_size ) != ESP_OK )
		{
			radio_msg_size 	= sizeof( radio_msg_default );
			radio_msg 		= realloc( radio_msg, radio_msg_size );
			if( radio_msg )
			{
				memcpy( radio_msg, &radio_msg_default, radio_msg_size );
				SaveTransponderData( radio_msg, radio_msg_size );
			}
			else
				radio_msg 	= &radio_msg_default[0][0];
		}
	}
}

/*
 * @brief Wifi AP initialization
 */
static void MyLaps_wifi_init()
{
	// init the wifi stuff here:
	tcpip_adapter_init();
	
	tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP);
	
	// assign a static IP to the network interface
	tcpip_adapter_ip_info_t 	info;
	
    memset(&info, 0, sizeof(info));
	
	IP4_ADDR(&info.ip, 192, 168, 10, 1);
    IP4_ADDR(&info.gw, 192, 168, 10, 1);
    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
	
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info);
	
	esp_event_loop_init(event_handler, NULL);
	wifi_init_config_t wifiInitializationConfig = WIFI_INIT_CONFIG_DEFAULT();
 
	esp_wifi_init(&wifiInitializationConfig);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);

	esp_wifi_set_mode(WIFI_MODE_AP );		// we will be a access point and able to connect to a wifi
	esp_wifi_set_config(WIFI_IF_AP, &apConfig);
	esp_wifi_start();
	//wifi_softap_dhcps_start();
}

static void dumpStatus(rmt_channel_t channel) 
{
bool 				loop_en;
uint8_t 			div_cnt;
uint8_t 			memNum;
bool 				lowPowerMode;
rmt_mem_owner_t		owner;
uint16_t 			idleThreshold;
uint32_t 			status;
rmt_source_clk_t 	srcClk;

	rmt_get_tx_loop_mode	(channel, &loop_en);
	rmt_get_clk_div			(channel, &div_cnt);
	rmt_get_mem_block_num	(channel, &memNum);
	rmt_get_mem_pd			(channel, &lowPowerMode);
	rmt_get_memory_owner	(channel, &owner);
	rmt_get_rx_idle_thresh	(channel, &idleThreshold);
	rmt_get_status			(channel, &status);
	rmt_get_source_clk		(channel, &srcClk);
	
	ESP_LOGI(MYLAPS_TAG, "Status for RMT channel 		%d", channel);
	ESP_LOGI(MYLAPS_TAG, "- Loop enabled: 		 		%d", loop_en);
	ESP_LOGI(MYLAPS_TAG, "- Clock divisor: 		 		%d", div_cnt);
	ESP_LOGI(MYLAPS_TAG, "- Number of memory blocks: 	%d", memNum);
	ESP_LOGI(MYLAPS_TAG, "- Low power mode: 			%d", lowPowerMode);
	ESP_LOGI(MYLAPS_TAG, "- Memory owner: 				%s", owner==RMT_MEM_OWNER_TX?"TX":"RX");
	ESP_LOGI(MYLAPS_TAG, "- Idle threshold: 			%d", idleThreshold);
	ESP_LOGI(MYLAPS_TAG, "- Status: 					%d", status);
	ESP_LOGI(MYLAPS_TAG, "- Source clock: 				%s", srcClk==RMT_BASECLK_APB?"APB (80MHz)":"1MHz");
}

/**
 * @brief RMT transmitter , this task will periodically send transponder telegram
 *
 */
static void rmt_mylap_tx_task()
{
	vTaskDelay(10);
	
	MyLaps_tx_init();
	
	esp_log_level_set(MYLAPS_TAG, ESP_LOG_INFO);
	
    int 		channel 	= RMT_TX_CHANNEL;
	int 		item_num 	= TELEGRAM_SIZE ;
	
	rmt_current 		= (rmt_item16_t*) malloc(size);
	
	if( rmt_current == NULL )
	{
		ESP_LOGI(MYLAPS_TAG, "ALLOCATION ERROR, Application Quit");
		return;
	}
	
	memset((void*) rmt_current, 0, size);
	
	item_num = MyLaps_CreateTelegram(channel, rmt_current , item_num,radio_msg);
	
	ESP_LOGI(MYLAPS_TAG, "RMT TX DATA");
	
	ESP_LOGI(MYLAPS_TAG, "rmt_set_mem_block_num        %d  ",rmt_set_mem_block_num	( channel, 8 ) 														);
	ESP_LOGI(MYLAPS_TAG, "rmt_write_items              %d  ",rmt_write_items		( channel, (rmt_item32_t *) rmt_current, (item_num + 1) / 2, true	));	// it is base on two half !
	ESP_LOGI(MYLAPS_TAG, "rmt_set_tx_loop_mode         %d  ",rmt_set_tx_loop_mode	( channel, true)													);
	ESP_LOGI(MYLAPS_TAG, "rmt_tx_start                 %d  ",rmt_tx_start			( channel, true )													);
	
	
	
	for(;;) 
	{
        //To send data according to the waveform items.
        vTaskDelay(10);
		rmt_delay->duration = 800 + ( esp_random() % 4096 );
		rmt_fill_tx_items( channel,(rmt_item32_t *)&rmt_current[item_num & 0xfffe],1,item_num / 2 );
//		rmt_tx_start( channel, true );
	//	rmt_write_items(channel, (rmt_item32_t *) rmt_current, item_num, false);
    }
    ESP_LOGI(MYLAPS_TAG, "END OF Transponder Application");
    free( rmt_current );
    vTaskDelete(NULL);
}

static void wifi_mylap_tx_task()
{
	vTaskDelay(500);
	
	MyLaps_wifi_init();
	
	esp_log_level_set(MYLAPS_TAG, ESP_LOG_INFO);
	
	ESP_LOGI(MYLAPS_TAG, "Wifi Mylaps started");

	
	for(;;) 
	{
        vTaskDelay( 10 );
    }
    ESP_LOGI(MYLAPS_TAG, "END OF Wifi Application");
    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(rmt_mylap_tx_task, "MyLaps Transponder Task", 2048, NULL, 10, NULL);
    xTaskCreate(wifi_mylap_tx_task, "MyLaps Wifi Task", 2048, NULL, 10, NULL);
}
