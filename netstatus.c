#include "netstatus.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<string.h>
#include <unistd.h>
#define TEST_HOST "google.com"   // Primary test site
#define ALT_HOST "cloudflare.com" // Alternative site

// Function to check internet connectivity
int check_connection(const char *host) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));  // Ensure all fields are initialized
	hints.ai_family = AF_INET;  // Use IPv4
	hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, "80", &hints, &res) == 0) {
        freeaddrinfo(res);
        return 1;  // Connected
    }
    return 0; // Disconnected
}

// Function to get public IP using curl
void get_public_ip() {
    printf("Fetching public IP...\n");
    system("curl -s ifconfig.me"); // Fetch and print public IP
    printf("\n");
}

// Function to get network interface info
void get_interface_info() {
    printf("\nNetwork Interface Details:\n");
    system("ip -o -4 addr show | awk '{print $2, $4}'"); // Get interface names & IPs
}

// Function to check latency (ping time)
void check_latency() {
    printf("\nChecking ping latency...\n");
    system("ping -c 3 google.com | tail -2");  // Show ping results
}
