#include <UserButton.h>
#include "PingPong.h"

module PingPongC{
	uses{	
		interface Timer<TMilli>;
		interface Leds;
		interface Notify<button_state_t>;
		interface Boot;	
	}
		
	uses{	//interfaces for Radio communication
		interface SplitControl;
		interface AMPacket;
		interface Packet; //not needed	
		interface AMSend;
		interface Receive;
	}
}

implementation{
	bool radioIsBusy = FALSE;
	bool playing = FALSE;
	message_t packet;
	nx_uint8_t timer_message;
	
	void sendMessage(nx_uint8_t messageCode){
		//initialize and send message
		if(!radioIsBusy){
			RadioMsg *ptr = (RadioMsg*) (call Packet.getPayload( &packet, sizeof(RadioMsg)));
			ptr->moteID = TOS_NODE_ID;
			ptr->data = messageCode;
			
			if (call AMSend.send(AM_BROADCAST_ADDR, &packet, sizeof(RadioMsg)) == SUCCESS) {
      			radioIsBusy = TRUE;
    		} 
		}	
	}
	
	void endGame(){
		call Timer.stop();	
		playing = FALSE;
		call Leds.set(7);     //Set all 3 LEDS to indicate the end of the game 
	}
	
	event void Boot.booted(){
	  	call SplitControl.start();	//turn radio on
  	}

	event void Notify.notify(button_state_t val){
		if (val == 1){
			if(playing == FALSE){
				playing = TRUE;
				timer_message = MESSAGE_START;
				call Leds.set(1);       //set only LED 0
				call Timer.startOneShot(TIMER_FREQ);
				//timer_message = MESSAGE_PLAY;
			} else{		
				endGame();	
				sendMessage(MESSAGE_STOP);
			}
		}
  	}

  	event void Timer.fired(){
   		call Leds.led0Off();
    	sendMessage(timer_message);			
  	}

	event void SplitControl.startDone(error_t error){
		if (error == SUCCESS){	
			call Notify.enable();
		} else{
			call SplitControl.start();	//if radio fails to turn on try again	
		}
	}

	event void SplitControl.stopDone(error_t error){
		//something something dark side of the force
	}

	event void AMSend.sendDone(message_t *msg, error_t error){
		if(&packet == msg){
			radioIsBusy = FALSE;
		}
	}

	event message_t * Receive.receive(message_t *msg, void *payload, uint8_t len){
		if (len == sizeof(RadioMsg)) {
			RadioMsg* ptr = (RadioMsg*)payload;
			
			if(ptr->data == MESSAGE_START){
				playing = TRUE;
				timer_message = MESSAGE_PLAY;
				call Leds.set(1);       //set only LED 0
				call Timer.startOneShot(TIMER_FREQ);
			} else if(ptr->data == MESSAGE_PLAY && playing == TRUE){
				timer_message = MESSAGE_PLAY;
				call Leds.led0On();
				call Timer.startOneShot(TIMER_FREQ);
			} else
				endGame();
			}
		return msg;

	}
	
}

