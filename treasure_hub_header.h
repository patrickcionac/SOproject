#ifndef HEADER_H
#define HEADER_H

void start_monitor();
void send_arg(const char *arg);
void send_signal();
void stop_monitor();
void handle_sigusr1(int sig);
void monitor_connect();
void handle_sigchld();

#endif