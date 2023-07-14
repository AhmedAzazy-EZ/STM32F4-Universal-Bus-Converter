#ifndef _OBSERVER_PATTERN_H_
#define _OBSERVER_PATTERN_H_

#include <vector>

class BASE_COM_t;

class send_handle
{
public:
	send_handle(BASE_COM_t * source);
	~send_handle();
	unsigned int data_received_tracker;
	unsigned int data_to_be_sent_tracker;
	BASE_COM_t * Source_COM;
};

class observer
{
public :
	observer();
  ~observer();
	void notify(BASE_COM_t * source , int receive_tracker);
	
		std::vector<send_handle *> obsrvables_tracking;
};


class subject
{
public:
		subject();
    ~subject();
		send_handle * attatch(BASE_COM_t * attatch , BASE_COM_t * to);
		std::vector<BASE_COM_t *> _observers;
		//void detachattatch(BASE_COM_t * Observer);
protected:
	void Notify_observers(BASE_COM_t * source);
private :
};


#endif