#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <hiredis/hiredis.h>

/*
static int validate_url_rediss(char *buf, int len)
{
	int err = 0;
	return 1;
}
*/

static int validate_url_redis(char *buf, int len, int secure)
{
	int err = 0;
	int db = 0;
	char *opt_str = NULL;
	char *auth_str = NULL;
	char *db_str = NULL;

//	Look for question mark
	char *qm = strchr(buf, '?');
	if (qm) {
		*qm = 0;
		opt_str = qm+1;
		if(strlen(opt_str) < 3) { printf("opt_str not valid!\n"); return err; }
	}

//	Look for auth string (user_str:password)
	char *at = strchr(buf, '@');
	if (at) {
		*at = 0;
		auth_str = buf;
		buf = at+1;
		if(strlen(auth_str) < 1) { printf("auth_str not valid!\n"); return err; }
	}

//	Look for question mark
	char *slash = strchr(buf, '/');
	if (slash) {
		*slash = 0;
		db_str = slash+1;
		if(strlen(db_str) < 1) { printf("db_str not valid!\n"); return err; }
		db = atoi(db_str);
	}

	len = strlen(buf);
	if(len < 1) { printf("location not valid!\n"); return err; }

	if(auth_str) { printf("auth_str: %s\n", auth_str); }
	printf("location: %s\n", buf);
	if(db_str) { printf("db_str: %s(%d)\n", db_str, db); }
	if(opt_str) { printf("opt_str: %s\n", opt_str); }
	printf("\n");

	return 1;
}

static int validate_url_unix(char *buf, int len)
{
	int err = 0;
	char *opt_str = NULL;
	char *user_str = NULL;

//	Look for question mark
	char *qm = strchr(buf, '?');
	if (qm) {
		*qm = 0;
		opt_str = qm+1;
		if(strlen(opt_str) < 3) { printf("opt_str not valid!\n"); return err; }
	}

//	Look for user_str
	char *at = strchr(buf, '@');
	if (at) {
		*at = 0;
		user_str = buf;
		buf = at+1;
		if(strlen(user_str) < 1) { printf("user_str not valid!\n"); return err; }
	}

	len = strlen(buf);
	if(strlen(buf) < 1) { printf("location not valid!\n"); return err; }

	if(user_str) { printf("user_str: %s\n", user_str); }
	printf("location: %s\n", buf);
	if(opt_str) { printf("opt_str: %s\n", opt_str); }
	printf("\n");

	return 1;
}

static int validate_url(char *buf, int len)
{
	int err = 0;
	printf("url: %s\n", buf);

//	Check Scheme
	if (strncmp(buf, "unix://", 7) == 0) {
		buf += 7; len -= 7;
		return validate_url_unix(buf, len);
	} else if (strncmp(buf, "redis://", 8) == 0) {
		buf += 8; len -= 8;
		return validate_url_redis(buf, len, 0);
	} else if (strncmp(buf, "rediss://", 9) == 0) {
		buf += 9; len -= 9;
		return validate_url_redis(buf, len, 1);
	}

	printf("%s is not a valid URL!\n", buf);
	return err;
}

static void redis_decode_test(char *url)
{
	int valid = 0;
	int len = strlen(url);
	if(len < 7) { return; }
	char *buf = calloc(1, len+1);
	memcpy(buf, url, len);
	valid = validate_url(buf, len);
	if(valid == 0) { fprintf(stderr, "NOT VALID\n"); exit(1); }
	free(buf);
}

int main(int argc, char *argv[])
{
	redis_decode_test("");

	redis_decode_test("unix:///path/to/socket.sock");
	redis_decode_test("unix://user_str@/path/to/socket.sock");
	redis_decode_test("unix:///path/to/socket.sock?db=0");
	redis_decode_test("unix://user_str@/path/to/socket.sock?db=0");
	redis_decode_test("unix:///path/to/socket.sock?db=0&password=password");
	redis_decode_test("unix://user_str@/path/to/socket.sock?db=0&password=password");

	redis_decode_test("redis://user:secret@localhost:6379/1?foo=bar&qux=baz");
	redis_decode_test("redis://user:secret@localhost:6379/2");
	redis_decode_test("redis://user:secret@localhost:6379");
	redis_decode_test("redis://localhost:6379");
	redis_decode_test("redis://localhost:6379/3");
	redis_decode_test("redis://localhost:6379/4?foo=bar&qux=baz");
	redis_decode_test("redis://pass@localhost:6379");
	redis_decode_test("redis://localhost:6379?foo=bar&qux=baz");

	return 0;
}
