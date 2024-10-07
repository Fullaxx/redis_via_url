#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int validate_url_rediss(char *buf, int len)
{
	return 1;
}

static int validate_url_redis(char *buf, int len)
{
	return 1;
}

static int validate_url_unix(char *buf, int len)
{
	char *options = NULL;
	char *username = NULL;
	printf("%s\n", buf);

//	Look for question mark
	char *qm = strchr(buf, '?');
	if (qm) {
		*qm = 0;
		options = qm+1;
		if(strlen(options) < 3) { printf("options not valid!\n"); return 1; }
	}

//	Look for username
	char *at = strchr(buf, '@');
	if (at) {
		*at = 0;
		username = buf;
		buf = at+1;
		len = strlen(buf);
		if(strlen(username) < 1) { printf("username not valid!\n"); return 1; }
	}

	if(strlen(buf) < 1) { printf("location not valid!\n"); return 1; }

	if(username) { printf("username: %s\n", username); }
	printf("location: %s\n", buf);
	if(options) { printf("options: %s\n", options); }
	printf("\n");

	return 1;
}

static int validate_url(char *buf, int len)
{
//	Check Scheme
	if (strncmp(buf, "unix://", 7) == 0) {
		printf("scheme: unix\n");
		buf += 7; len -= 7;
		return validate_url_unix(buf, len);
	} else if (strncmp(buf, "redis://", 8) == 0) {
		printf("scheme: redis\n");
		buf += 8; len -= 8;
		return validate_url_redis(buf, len);
	} else if (strncmp(buf, "rediss://", 9) == 0) {
		printf("scheme: rediss\n");
		buf += 9; len -= 9;
		return validate_url_rediss(buf, len);
	}

	printf("%s is not a valid URL!\n", buf);
	return 0;
}

static void redis_decode_test(char *url)
{
	int len = strlen(url);
	if(len < 7) { return; }
	char *buf = calloc(1, len+1);
	memcpy(buf, url, len);
	validate_url(buf, len);
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
