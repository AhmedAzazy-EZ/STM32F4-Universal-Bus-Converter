#ifndef _OBSERVER_PATTERN_H_
#define _OBSERVER_PATTERN_H_


#ifndef __cplusplus
#define __cplusplus
#endif

#include "COM_Generic.h"
#include <vector>


class observer
{
public :
	void notify(BASE_COM_t * )
}


class subject
{
public:
		
		void attatch(BASE_COM_t * Observer);
		void detachattatch(BASE_COM_t * Observer);
protected:
	void notify();
private :
	std::vector<BASE_COM_t *> _observers;
};


#endif