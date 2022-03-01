#include <event2/event.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{	
	int i;
	const char * ptr;
	struct event_base * base = event_base_new();
	
	ptr = event_base_get_method(base);
	
	printf("%s\n", ptr);

	event_base_free(base);

	return 0;
}
