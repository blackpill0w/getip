/*
** Little program that displays IP (v4 & v6 if available) address of a URL.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

void help() {
   printf(
      "Usage: getip URL [-4|-6]\n"
      "   -h, --help: Displays this message\n"
      "   -4        : Display IPv4 only\n"
      "   -6        : Display IPv6 only\n"
      );
}

int main(int argc, char **argv) {
   bool only4 = false;
   bool only6 = false;
   char *url = NULL;
   if (argc < 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
      help();
      exit(0);
   }

   for (int i = 1; i < argc; ++i) {
      if (!strcmp(argv[i], "-4")) only4 = true;
      else if (!strcmp(argv[i], "-6")) only6 = true;
      else url = argv[i];
   }
   if (url == NULL) {
      help();
      exit(1);
   }
   if (only4 && only6) {
      fprintf(stderr, "Nothing to display\n");
      exit(1);
   }

   struct addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   struct addrinfo *res;

   if (getaddrinfo(url, "http", &hints, &res)) {
      fprintf(stderr, "Invalid URL/IP or connection problem: %s\n", url);
      exit(1);
   };

   for (struct addrinfo *curr = res; curr != NULL; curr = curr->ai_next) {
      if (only4 && curr->ai_family == AF_INET6) continue;
      if (only6 && curr->ai_family == AF_INET) continue;
      char buf[INET6_ADDRSTRLEN] = {0};
      inet_ntop(curr->ai_family, &curr, buf, curr->ai_addrlen);
      printf("%s\n", buf);
   }

   freeaddrinfo(res);
   return 0;
}
