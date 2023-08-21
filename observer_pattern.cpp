#include "observer_pattern.h"
#include "BASE_COM.h"


send_handle::send_handle(BASE_COM_t * source)
{
	data_received_tracker = source->receive_tracker;
	data_to_be_sent_tracker = source->receive_tracker;
	Source_COM = source;
}

subject::~subject()
{
	delete this;
}


send_handle * subject::attatch(BASE_COM_t * attatch , BASE_COM_t * to)
{
	_observers.push_back(attatch);
	send_handle * ptr = new send_handle(to);
	return ptr;
}


void subject::Notify_observers(BASE_COM_t * source)
{
	for(BASE_COM_t * item : _observers)
	{
		item->notify(source , source->receive_tracker);
	}
}

void observer::notify(BASE_COM_t * source , int receive_tracker)
{
	for(send_handle * item : obsrvables_tracking)
	{
		if((unsigned long)item->Source_COM == (unsigned long)source)
		{
			item->data_received_tracker = receive_tracker;
			return;
		}
	}
}

send_handle::~send_handle()
{

}

observer::observer()
{

}

observer::~observer()
{

}

subject::subject()
{
	
}


