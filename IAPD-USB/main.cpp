#include <stdio.h>
#include "USBEnumerator.h"

int main()
{
	USBEnumerator enumerator;
	enumerator.getVectorOfDevices();
    return 0;
}

