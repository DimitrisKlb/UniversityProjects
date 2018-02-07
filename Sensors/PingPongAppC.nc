configuration PingPongAppC{
}

implementation{
	components MainC, PingPongC, LedsC, UserButtonC;
	components new TimerMilliC();
	components ActiveMessageC;
	components new AMSenderC(AM_RADIO);
	components new AMReceiverC(AM_RADIO);

	PingPongC.Timer -> TimerMilliC;
	PingPongC.Leds -> LedsC;
	PingPongC.Boot -> MainC;
	PingPongC.Notify -> UserButtonC;
	
	PingPongC.SplitControl -> ActiveMessageC;
	PingPongC.AMSend -> AMSenderC;
	PingPongC.AMPacket -> AMSenderC;
	PingPongC.Packet -> AMSenderC;
	PingPongC.Receive -> AMReceiverC;
}

