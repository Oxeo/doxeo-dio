#ifndef OXEODIO_H
#define OXEODIO_H

class OxeoDio {

  public:
    OxeoDio();
	void setSenderPin(int pin);
	void setReceiverPin(int pin);
	unsigned long read();
	void send(unsigned long data);

  private:
	int _senderPin;
	int _receiverPin;
};

#endif // OXEODIO_H

