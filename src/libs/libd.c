#include "libd.h"

volatile int TERM = FALSE;
const char *E = "[X]";
const char *W = "[!]";
const char *I = "[*]";

const char* err() {
    return E;
}

const char* warn() {
    return W;
}

const char* info() {
    return I;
}

/* Daemon initialization */

/*
 *  Kills the daemon process when a termination signal is received.
 *
 *  @param signum Signal number
 *  @return void
 *
 *  Should be used via signal(2) to handle SIGINT and SIGTERM.
 */
static void handle_termination(int signum) {
    TERM = TRUE;
}

/*
 *  Resets all signal handlers to their default values.
 *
 *  @return void
 *
 *  This function is used to prevent the daemon from handling signals that were not explicitly set.
 */
static void reset_signal_handlers() {
#if defined(_NSIG)
    uint i;
    for (i = 1; i < _NSIG; i++) {
        if (i != SIGKILL && i != SIGSTOP) {
            signal(i, SIG_DFL);
        }
    }
#endif
}

/*
 *  Clears the signal mask.
 *
 *  @return TRUE if successful, FALSE if sigemptyset(3) fails
 *
 *  This function is used to prevent the daemon from blocking signals that were not explicitly set.
 */
static int clear_signal_mask() {
    sigset_t mask;
    return ((sigemptyset(&mask) == 0)
        && (sigprocmask(SIG_SETMASK, &mask, NULL) == 0));
}

/*
 *  Closes all file descriptors except for stdin, stdout, and stderr.
 *
 *  @return TRUE if successful, FALSE if getrlimit(2) fails
 *
 *  This function is used to prevent the daemon from holding open inherited file descriptors.
 */
static uint close_fds() {
    uint i;
    struct rlimit rlim;
    int fds = getrlimit(RLIMIT_NOFILE, &rlim);

    if (fds == -1) {
        return FALSE;
    }

    for (i = 3; i < fds; i++) {
        close(i);
    }

    return TRUE;
}

/*
 *  Preps the environment for the daemon process. 
 *
 *  @return void
 *
 *  This function is used to prevent the daemon from inheriting the parent process's environment.
 *  It makes the process fork SysV-compatible.
 */
void prep_env() {
    reset_signal_handlers();
    if (clear_signal_mask() == FALSE) {
        fprintf(stderr, "%s Can't clear signal mask; daemon creation unsafe.\n", E);
        exit(1);
    }
    if (close_fds() == FALSE) {
        fprintf(stderr, "%s Can't close file descriptors; daemon creation unsafe.\n", E);
        exit(1);
    }
    if (clearenv()) {
        fprintf(stderr, "%s Can't clear environment; daemon creation unsafe.\n", E);
        exit(1);
    }
}

/*
 *  Attaches file descriptors to /dev/null.
 *
 *  @return TRUE if successful, FALSE if open(2) or dup2(2) fails
 *
 *  This function is used to prevent the daemon from writing to the terminal.
 */
static int attach_fds_to_null() {
    //int null_fd_read, null_fd_write;

    //return (((null_fd_read = open("/dev/null", O_RDONLY)) != -1)
    //    && ((null_fd_write = open("/dev/null", O_WRONLY)) != -1)
    //    && (dup2(null_fd_read, STDIN_FILENO) != -1)
    //    && (dup2(null_fd_write, STDOUT_FILENO) != -1)
    //    && (dup2(null_fd_write, STDERR_FILENO) != -1));

    return TRUE;
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = size * nmemb;
    memcpy(stream, ptr, written + 1);
    return written;
}

/* Main loop */

/*
 *  Main loop of the daemon process.
 *
 *  @param symbol_pair Symbol pair to track
 *  @param interval Interval in seconds to track the symbol pair
 *  @return void
 *
 *  This function should be called after init_daemon() to start the main loop of the daemon process.
 */
static void main_loop(char *symbol_pair, uint interval) {
    char buffer[1024];

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if (curl == NULL) {
        fprintf(stderr, "%s Can't initialize cURL; daemon creation failed.\n", E);
        exit(1);
    }

    char url[64];
    sprintf(url, "https://api.binance.com/api/v3/ticker/price?symbol=%s", symbol_pair);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

    char filename[32];
    sprintf(filename, "/tmp/tikrtrakrd.%s.json", symbol_pair);

    fprintf(stderr, "%s Checking if file %s exists\n", I, filename);
    FILE *fd = fopen(filename, "r");
    if (fd != NULL) {
        fprintf(stderr, "%s File %s already exists; daemon creation failed.\n", E, filename);
        fclose(fd);
        exit(1);
    }

    fprintf(stderr, "%s Creating file %s\n", I, filename);
    fd = fopen(filename, "w");
    if (fd == NULL) {
        fprintf(stderr, "%s Can't create file %s; daemon creation failed.\n", E, filename);
        exit(1);
    }
    fclose(fd);

    while (!TERM) {
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "%s Can't fetch data from Binance; keeping old data.\n", W);
        } else {
            fd = fopen(filename, "w");
            fprintf(fd, "%s", buffer);
            fclose(fd);
        }
        sleep(interval);
    }
    curl_easy_cleanup(curl);
    fprintf(stderr, "%s Daemon process terminated\n", I);

    /* Cleanup */
    remove(filename);
}

/*
 *  Initializes the daemon process.
 *
 *  @param symbol_pair Symbol pair to track
 *  @param interval Interval in seconds to track the symbol pair
 *  @return void
 *
 *  This function should not be called. Use `init_helper()` to start the daemon process.
 */
static void init_daemon(char *symbol_pair, uint interval) {
    signal(SIGINT, handle_termination);
    signal(SIGTERM, handle_termination);

    if (attach_fds_to_null() == FALSE) {
        fprintf(stderr, "%s Can't attach file descriptors to /dev/null; daemon creation failed.\n", E);
        exit(1);
    }

    umask(0);

    if (chdir("/") == -1) {
        fprintf(stderr, "%s Can't change working directory to root; daemon creation failed.\n", E);
        exit(1);
    }
    
    fprintf(stderr, "%s Daemon process initialized\n", I);
    main_loop(symbol_pair, interval);
}

/*
 *  Initializes the daemon process via a helper process. Entry point for the daemon.
 *
 *  @param symbol_pair Symbol pair to track
 *  @param interval Interval in seconds to track the symbol pair
 *  @return void
 *
 *  This function should be called before the main loop of the daemon process.
 *  It does all the necessary setup for the daemon environment.
 */
void init_helper(char *symbol_pair, uint interval) {
    if (setsid() == -1) {
        fprintf(stderr, "%s Can't create new session; daemon creation failed.\n", E);
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "%s Can't fork daemon process; daemon creation failed.\n", E);
        exit(1);
    } else if (pid == 0) {
        fprintf(stderr, "%s Daemon process started\n", I);
        init_daemon(symbol_pair, interval);
    }

    exit(0);
}
