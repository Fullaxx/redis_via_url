#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void redis_url_decode(char *buf, int len)
{
	char *options = NULL;
	char *username = NULL;
	printf("%s\n", buf);

//	Decode Protocol
	if (strncmp(buf, "unix://", 7) == 0) {
		printf("protocol: unix\n");
		buf += 7; len -= 7;
	} else if (strncmp(buf, "redis://", 8) == 0) {
		printf("protocol: redis\n");
		buf += 8; len -= 8;
	} else if (strncmp(buf, "rediss://", 9) == 0) {
		printf("protocol: rediss\n");
		buf += 9; len -= 9;
	} else {
		printf("ERROR\n");
		return;
	}

//	Look for question mark
	char *qm = strchr(buf, '?');
	if (qm) {
		*qm = 0;
		options = qm+1;
	}

//	Look for username
	char *at = strchr(buf, '@');
	if (at) {
		*at = 0;
		username = buf;
		buf = at+1;
		len = strlen(buf);
	}

	if(username) { printf("username: %s\n", username); }
	printf("location: %s\n", buf);
	if(options) { printf("options: %s\n", options); }
	printf("\n");
}

static void redis_decode_test(char *url)
{
	int len = strlen(url);
	if(len < 7) { return; }
	char *buf = calloc(1, len+1);
	memcpy(buf, url, len);
	redis_url_decode(buf, len);
	free(buf);
}

int main(int argc, char *argv[])
{
	redis_decode_test("");

	redis_decode_test("unix:///path/to/socket.sock");

	redis_decode_test("unix://username@/path/to/socket.sock");

	redis_decode_test("unix:///path/to/socket.sock?db=0");

	redis_decode_test("unix://username@/path/to/socket.sock?db=0");

	redis_decode_test("unix:///path/to/socket.sock?db=0&password=password");

	redis_decode_test("unix://username@/path/to/socket.sock?db=0&password=password");

	return 0;
}
