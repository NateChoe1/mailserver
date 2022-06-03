#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>

static int addemail(int argc, char **argv);
static int updateemail(int argc, char **argv);
static int deleteemail(int argc, char **argv);
static int showemails(int argc, char **argv);
static void getcredentials(int argc, char **argv,
		char **emailret, char **passret);
static char *askuser(char *prompt);
static void hideinput();
static void showinput();
static void *xmalloc(size_t size);
static void *xrealloc(void *ptr, size_t size);
static int exec(char *pathname, char *arg, ...);

#define INPUT_FLAGS (ECHO)

int main(int argc, char **argv) {
	if (argc < 2) {
		printf(
"Usage: %s [command] args\n"
"Commands:\n"
"	add (email) (password) - Creates a new email\n"
"	update (email) (new password) - Changes a password (be courteous)\n"
"	del (email) - Removes an email (be really courteous)\n"
"	list - List all emails\n"
				, argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "add") == 0)
		return addemail(argc - 1, argv + 1);
	else if (strcmp(argv[1], "update") == 0)
		return updateemail(argc - 1, argv + 1);
	else if (strcmp(argv[1], "del") == 0)
		return deleteemail(argc - 1, argv + 1);
	else if (strcmp(argv[1], "list") == 0)
		return showemails(argc - 1, argv + 1);
	fprintf(stderr, "Unknown command %s\n", argv[1]);
	return 1;
}

static int addemail(int argc, char **argv) {
	char *email, *password;
	getcredentials(argc, argv, &email, &password);
	return exec("/usr/bin/docker", "docker", "exec", "mailserver",
			"setup", "email", "add", email, password, NULL);
}

static int updateemail(int argc, char **argv) {
	char *email, *password;
	getcredentials(argc, argv, &email, &password);
	return exec("/usr/bin/docker", "docker", "exec", "mailserver",
			"setup", "email", "update", email, password, NULL);
}

static int deleteemail(int argc, char **argv) {
	char *email;
	if (argc < 2)
		email = askuser("Email to delete: ");
	else
		email = argv[1];
	return exec("/usr/bin/docker", "docker", "exec", "mailserver",
			"setup", "email", "del", email, NULL);
}

static int showemails(int argc, char **argv) {
	return exec("/usr/bin/docker", "docker", "exec", "mailserver",
			"setup", "email", "list");
}

static void getcredentials(int argc, char **argv,
		char **emailret, char **passret) {
	if (argc < 2)
		*emailret = askuser("Email: ");
	else
		*emailret = argv[1];
	if (argc < 3) {
		hideinput();
		*passret = askuser("Password: ");
		showinput();
		putchar('\n');
	}
	else
		*passret = argv[2];
}

static char *askuser(char *prompt) {
	size_t len, alloc;
	char *ret;
	alloc = 80;
	ret = xmalloc(alloc);
	fputs(prompt, stdout);
	for (len = 0;; ++len) {
		int c;
		c = fgetc(stdin);
		if (len >= alloc) {
			alloc *= 2;
			ret = xrealloc(ret, alloc);
		}
		if (c == '\n')
			break;
		ret[len] = c;
	}
	ret[len] = '\0';
	return ret;
}

static void hideinput() {
	struct termios term;
	if (tcgetattr(0, &term) < 0) {
		fprintf(stderr, "tcgetattr() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	term.c_lflag &= ~INPUT_FLAGS;
	if (tcsetattr(0, TCSANOW, &term)) {
		fprintf(stderr, "tcsetattr() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void showinput() {
	struct termios term;
	if (tcgetattr(0, &term) < 0) {
		fprintf(stderr, "tcgetattr() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	term.c_lflag |= INPUT_FLAGS;
	if (tcsetattr(0, TCSANOW, &term)) {
		fprintf(stderr, "tcsetattr() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static int exec(char *pathname, char *arg, ...) {
	pid_t pid;
	int status;
	pid = fork();
	switch (pid) {
	case -1:
		return 1;
	case 0: {
		char **argv;
		size_t len, alloc;
		va_list ap;
		va_start(ap, arg);
		alloc = 10;
		argv = xmalloc(alloc * sizeof *argv);
		argv[0] = arg;
		for (len = 1;; ++len) {
			if (len >= alloc) {
				alloc *= 2;
				argv = xrealloc(argv, alloc * sizeof *argv);
			}
			argv[len] = va_arg(ap, char *);
			if (argv[len] == NULL)
				break;
		}
		va_end(ap);
		execv(pathname, argv);
		fprintf(stderr, "execl() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	default:
		waitpid(pid, &status, 0);
		return status;
	}
}

static void *xmalloc(size_t size) {
	void *ret;
	ret = malloc(size);
	if (ret == NULL) {
		fputs("xmalloc() failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ret;
}

static void *xrealloc(void *ptr, size_t size) {
	void *ret;
	ret = realloc(ptr, size);
	if (ret == NULL) {
		fputs("xrealloc() failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ret;
}
