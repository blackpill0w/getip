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
      "Usage: getip URL [-4|-6] [-p/--port port]\n"
      "   -h, --help: displays this help and exit\n"
      "   -p, --port: specify port (80 by default, http)\n"
      "   -4        : display IPv4 only\n"
      "   -6        : display IPv6 only\n"
      );
}

int main(int argc, char **argv) {
   bool only4 = false;
   bool only6 = false;
   char *url = NULL;
   char *port = "80";
   if (argc < 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
      help();
      exit(argc < 2 ? 1 : 0);
   }

   bool get_port = false;
   for (int i = 1; i < argc; ++i) {
      if (get_port) port = argv[i];
      else if (!strcmp(argv[i], "-4")) only4 = true;
      else if (!strcmp(argv[i], "-6")) only6 = true;
      else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) get_port = true, port = NULL;
      else url = argv[i];
   }
   if (url == NULL || port == NULL) {
      help();
      exit(1);
   }
   if (only4 && only6) {
      fprintf(stderr, "Nothing to display\n");
      exit(1);
   }

   struct addrinfo hints;
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = only4 ? AF_INET : only6 ? AF_INET6 : AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   struct addrinfo *res = NULL;
   if (getaddrinfo(url, port, &hints, &res)) {
      freeaddrinfo(res);
      fprintf(stderr, "Invalid URL/IP or connection problem: %s\n", url);
      exit(1);
   };

   for (struct addrinfo *curr = res; curr != NULL; curr = curr->ai_next) {
      char buf[INET6_ADDRSTRLEN] = {0};
      void *addr;
      if (curr->ai_family == AF_INET) {
         struct sockaddr_in *s = (struct sockaddr_in *) curr->ai_addr;
         addr = &(s->sin_addr);
      }
      else {
         if (curr->ai_family == AF_INET6) {
         struct sockaddr_in6 *s = (struct sockaddr_in6 *) curr->ai_addr;
         addr = &(s->sin6_addr);
      }
      }

      inet_ntop(curr->ai_family, addr, buf, sizeof(buf));
      printf("%s\n", buf);
   }

   freeaddrinfo(res);
   res = NULL;
   return 0;
}
