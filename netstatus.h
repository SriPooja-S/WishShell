#ifndef NETSTATUS_H
#define NETSTATUS_H

int check_connection(const char *host);
void get_public_ip();
void get_interface_info();
void check_latency();

#endif // NETSTATUS_H

