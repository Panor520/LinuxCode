#include <event2/event.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{	
	int i;
	const char ** ptr;
	struct event_base * base = event_base_new();
	
	ptr = event_get_supported_methods();
	
	for(i = 0; i < 5; i++)
		printf("%s\n", ptr[i]);

	event_base_free(base);

	return 0;
}
