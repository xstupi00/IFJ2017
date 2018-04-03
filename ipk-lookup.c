#include "ipk-lookup.h"

char dns_server_address[256];
int TIMEOUT = 5;
int record_type = 1;
bool iterative = false;
char domain_name[256];
int port_number = 53;

bool final_answer = 0;

void change_to_dns_format(unsigned char *dns, unsigned char *host) {
    int lock = 0, i;
    strcat((char *) host, ".");

    for (i = 0; i < strlen((char *) host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++)
                *dns++ = host[lock];
            lock++;
        }
    }
    *dns++ = '\0';
}

void change_to_ip_format(unsigned char *dns, unsigned char *host) {

    int end = strlen((char *)host)-1;
    for (int i = strlen((char *) host)-1; i >= 0; i--) {
        if (host[i] == '.') {
            int cnt = 0;
            for (int j = i + 1; j <= end; j++, cnt++)
                *dns++ = host[j];
            *dns++ = '.';
            end -= cnt+1;
        }
    }
    for(int j = 0; end >= 0; j++, end--)
        *dns++ = host[j];
    char *src = ".in-addr.arpa";
    for(int i = 0; i < strlen((char *)src); i++)
        *dns++ = src[i];

    *dns++ = '\0';
}

void change_to_ipv6_format(unsigned char *dns, unsigned char *host) {

    int count_colons = 0;
    for (int i = 0; i < strlen((char *)host); i++)
        if (host[i] == ':')
            count_colons++;
    int zero_blocks = 7 - count_colons;
    //printf("\n%d\n", zero_blocks);

    int count = 0;
    for (int i = strlen((char*)host)-1; i >= 0; i--) {
        if (host[i] != ':') {
            *dns++ = host[i];
            *dns++ = '.';
            count++;
        }
        else {
            while (count < 4) {
                *dns++ = '0';
                *dns++ = '.';
                count++;
            }
            if (host[i+1] == ':') {
                for (int cnt = zero_blocks * 4; cnt > 0; cnt--) {
                    *dns++ = '0';
                    *dns++ = '.';
                }
            }
            count = 0;
        }
    }

    char *src = "ip6.arpa";
    for(int i = 0; i < strlen((char *)src); i++)
        *dns++ = src[i];

    *dns++ = '\0';
}

char *decode_record_type(int type) {

    if (type == 1)
        return "A";
    else if (type == 2)
        return "NS";
    else if (type == 5)
        return "CNAME";
    else if (type == 12)
        return "PTR";
    else if (type == 28)
        return "AAAA";
}

u_char *read_name(unsigned char *reader, unsigned char *buffer, int *count) {

    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char *) malloc(256);

    name[0] = '\0';

    //read the names in 3www6google3com format
    while (*reader) {
        if (*reader >= 192) {
            offset = (*reader) * 256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        } else
            name[p++] = *reader;

        reader = reader + 1;

        if (!jumped)
            *count = *count + 1; //if we havent jumped to another location then we can count up
    }

    name[p] = '\0'; //string complete
    if (jumped == 1)
        *count = *count + 1; //number of steps we actually moved forward in the packet

    //now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int) strlen((const char *) name); i++) {
        p = name[i];
        for (j = 0; j < (int) p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0'; //remove the last dot

    return name;
}

char* my_get_host_by_name(char *server_address, stack_t *add_info, stack_t *servers_queue, char* my_domain_name, int my_record_type, int ip_flag, int recursion_flag) {

    unsigned char buffer[65536], *tmpname, *qname, *reader;
    int tmp, UDPSocket;

    struct sockaddr_in addr;
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct DATA_FORMAT answers[64], auth[64], addit[64];
    struct QUESTION_FORMAT *qinfo = NULL;


    UDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port_number);
    dest.sin_addr.s_addr = inet_addr(server_address);

    dns = (struct DNS_HEADER *) &buffer;
    dns->ID = (unsigned short) htons(getpid());
    dns->QR = 0;
    dns->AA = 0;
    dns->TC = 0;
    dns->RD = recursion_flag;
    dns->CD = 0;
    dns->AD = 0;
    dns->RA = 0;
    dns->Z = 0;
    dns->OPCODE = 0;
    dns->RCODE = 0;
    dns->QDCOUNT = htons(1);
    dns->ANCOUNT = 0;
    dns->ARCOUNT = 0;
    dns->NSCOUNT = 0;

    //printf("Sending to %s", server_address);

    qname = (unsigned char *) &buffer[sizeof(struct DNS_HEADER)];
    tmpname = (unsigned char *) malloc(sizeof(struct DNS_HEADER));
    if (my_record_type != 12)
        change_to_dns_format(qname, my_domain_name);
    else {
        int addr_mode = -1;
        for (int i = 0; i < strlen((char *)my_domain_name); i++) {
            if (my_domain_name[i] == '.') {
                change_to_ip_format(tmpname, my_domain_name);
                //printf("\n-%s\n", tmpname);
                change_to_dns_format(qname, tmpname);
                break;
            } else if (my_domain_name[i] == ':') {
                change_to_ipv6_format(tmpname, my_domain_name);
                change_to_dns_format(qname, tmpname);
                //printf("\n-------NEW: %s\n", qname);
                break;
            }
        }
    }


    //printf("\n-------NEW: %s\n", qname);

    qinfo = (struct QUESTION_FORMAT *) &buffer[sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1)];
    qinfo->QCLASS = htons(1);
    qinfo->QTYPE = htons(my_record_type);

    //printf("\nQuery type %d\n", htons(qinfo->QTYPE));
    //printf("What i want %s", qname);

    //printf("\nSending Packet...");
    if (sendto(UDPSocket, (char *) buffer,
               sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1) + sizeof(struct QUESTION_FORMAT), 0,
               (struct sockaddr *) &dest, sizeof(dest)) < 0)
        perror("sendto failed");
    //printf("Done");

    //printf("\nReceiving answer...");
    tmp = sizeof dest;
    if (recvfrom(UDPSocket, (char *) buffer, 65536, 0, (struct sockaddr *) &dest, (socklen_t *) &tmp) < 0)
        perror("recvfrom failed");
    //printf("Done\n");

    dns = (struct DNS_HEADER *) buffer;

    //printf("\nThe response contains : ");
    //printf("\n %d Questions.", ntohs(dns->QDCOUNT));
    //printf("\n %d Answers.", ntohs(dns->ANCOUNT));
    //printf("\n %d Authoritative Servers.", ntohs(dns->NSCOUNT));
    //printf("\n %d Additional records.\n\n", ntohs(dns->ARCOUNT));

    reader = &buffer[sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1) + sizeof(struct QUESTION_FORMAT)];

    int end = 0;

    for (int i = 0; i < ntohs(dns->ANCOUNT); i++) {
        if (ip_flag == 0)
            final_answer = 1;
        answers[i].NAME = read_name(reader, buffer, &end);
        reader += end;

        answers[i].RESOURCE = (struct RESOURCE_FORMAT *) (reader);
        if (ntohs(answers[i].RESOURCE->TYPE) != 28)
            reader += sizeof(struct RESOURCE_FORMAT);
        else
            reader += sizeof(struct RESOURCE_FORMAT)-8;

        if (ntohs(answers[i].RESOURCE->TYPE) == 1 || ntohs(answers[i].RESOURCE->TYPE) == 28) {
            answers[i].RDATA = (unsigned char *) malloc(ntohs(answers[i].RESOURCE->RDLENGTH));
            if(ntohs(answers[i].RESOURCE->TYPE) == 1) {
                for (int j = 0; j < ntohs(answers[i].RESOURCE->RDLENGTH); j++)
                    answers[i].RDATA[j] = reader[j];
                if (ip_flag == 1)
                    return answers[i].RDATA;
            }
            if(ntohs(answers[i].RESOURCE->TYPE) == 28)
                for (int j = 0; j < ntohs(answers[i].RESOURCE->RDLENGTH)+32; j++)
                    answers[i].RDATA[j] = reader[j];
            answers[i].RDATA[ntohs(answers[i].RESOURCE->RDLENGTH)] = '\0';
            reader += ntohs(answers[i].RESOURCE->RDLENGTH);
        } else {
            answers[i].RDATA = read_name(reader, buffer, &end);
            reader += end;
        }
    }

    //printf("\nAnswer Records : %d \n", ntohs(dns->ANCOUNT));
    char class[3];
    char str_record_type[6];
    for (int i = 0; i < ntohs(dns->ANCOUNT); i++) {
        if (ntohs(answers[i].RESOURCE->CLASS) == 1)
            strcpy(class, "IN");
        strcpy(str_record_type, decode_record_type(ntohs(answers[i].RESOURCE->TYPE)));
        printf("%s %s %s ", answers[i].NAME, class, str_record_type);
        if (ntohs(answers[i].RESOURCE->TYPE) == 1) {
            long *p;
            p = (long *) answers[i].RDATA;
            addr.sin_addr.s_addr = (*p);
            printf("%s", inet_ntoa(addr.sin_addr));
        }

        if (ntohs(answers[i].RESOURCE->TYPE) == 5 || ntohs(answers[i].RESOURCE->TYPE) == 12)
            printf("%s", answers[i].RDATA);

        if (ntohs(answers[i].RESOURCE->TYPE) == 28) {
            //long long *p;
            //p = (long long *) answers[i].RDATA;
            struct sockaddr_in6 *ipv6;
            //memcpy(addr6->sin6_addr.s6_addr, p, sizeof(p));
            char add[INET6_ADDRSTRLEN];
            ipv6 = (struct sockaddr_in6 *)answers[i].RDATA;
            void *addr1;
            addr1 = &(ipv6->sin6_addr);
            inet_ntop(AF_INET6, addr1, add, sizeof(add));
            printf("%s", add);
        }

        printf("\n");
    }

    for (int i = 0; i < ntohs(dns->NSCOUNT); i++) {
        auth[i].NAME = read_name(reader, buffer, &end);
        reader += end;
        auth[i].RESOURCE = (struct RESOURCE_FORMAT *) reader;
        reader += sizeof(struct RESOURCE_FORMAT);
        auth[i].RDATA = read_name(reader, buffer, &end);
        reader += end;
        if (ip_flag == 0 && ntohs(dns->NSCOUNT) != 1) {
            S_Push(servers_queue, &auth[i]);
        }
    }

    //printf("\nAuthoritive Records : %d \n" , ntohs(dns->NSCOUNT) );
    for(int i = 0; i < ntohs(dns->NSCOUNT); i++) {
        //printf("Name : %s ",auth[i].NAME);
        if(ntohs(auth[i].RESOURCE->TYPE) == 2) {
        //    printf("has nameserver : %s",auth[i].RDATA);
        }
        //printf("\n");
    }

    for (int i = 0; i < ntohs(dns->ARCOUNT); i++) {
        addit[i].NAME = read_name(reader, buffer, &end);
        reader += end;

        addit[i].RESOURCE = (struct RESOURCE_FORMAT*)reader;
        reader += sizeof(struct RESOURCE_FORMAT);

        if(ntohs(addit[i].RESOURCE->TYPE) == 1) {
            addit[i].RDATA = (unsigned char*)malloc(ntohs(addit[i].RESOURCE->RDLENGTH));
            for (int j = 0; j < ntohs(addit[i].RESOURCE->RDLENGTH); j++)
                addit[i].RDATA[j]= reader[j];
            addit[i].RDATA[ntohs(addit[i].RESOURCE->RDLENGTH)] = '\0';
            reader += ntohs(addit[i].RESOURCE->RDLENGTH);
        }
        else if (ntohs(addit[i].RESOURCE->TYPE) == 28)
            reader += ntohs(addit[i].RESOURCE->RDLENGTH);
        else {
            addit[i].RDATA = read_name(reader, buffer, &end);
            reader += end;
        }
    }

    //printf("\nAdditional Records : %d \n" , ntohs(dns->ARCOUNT) );
    for (int i=0; i < ntohs(dns->ARCOUNT); i++) {
        //printf("Name : %s ",addit[i].NAME);
        if(ntohs(addit[i].RESOURCE->TYPE) == 1) {
            long *p;
            p=(long*)addit[i].RDATA;
            addr.sin_addr.s_addr=(*p);
            //printf("has IPv4 address : %s",inet_ntoa(addr.sin_addr));
            S_Push(add_info, addit[i].RDATA);
            S_Push(add_info, addit[i].NAME);
        }
        if (ntohs(addit[i].RESOURCE->TYPE) == 28)
            continue;
            //printf("has IPv6 address:");
        if (ntohs(addit[i].RESOURCE->TYPE) == 12)
            continue;
            //printf("has CNAME:");
        //printf("\n");
    }

    return NULL;
}

void iterative_query() {

    char class[3];
    char str_record_type[6];
    //char *last_auth_server = dns_server_address;
    stack_t *servers_queue = S_Init();
    stack_t *add_info = S_Init();
    char *tmp = my_get_host_by_name(dns_server_address, add_info, servers_queue, domain_name, record_type, 0, 0);
    add_info = S_Save(add_info);

    while (final_answer == 0) {
        struct DATA_FORMAT *auth_server;
        if (S_Empty(servers_queue))
            break;
        auth_server = (struct DATA_FORMAT *) S_Top(servers_queue);
        if (ntohs(auth_server->RESOURCE->CLASS) == 1)
            strcpy(class, "IN");
        strcpy(str_record_type, decode_record_type(ntohs(auth_server->RESOURCE->TYPE)));
        S_Pop(servers_queue);printf("%s %s %s %s\n", auth_server->NAME, class, str_record_type, auth_server->RDATA);
        //printf("STACK: %s\n", auth_server->RDATA);

        while (!S_Empty(add_info)) {
            char *server = (char *) S_Top(add_info);
            //printf("1.%s\n2.%s\n", server, auth_server->RDATA);
            if (!strcmp(server, auth_server->RDATA)) {
                //printf("1.%s\n2.%s\n", server, auth_server->RDATA);
                S_Pop(add_info);
                break;
            }
            S_Pop(add_info);
            S_Pop(add_info);
        }

        long *p;
        struct sockaddr_in addr;
        if (S_Empty(add_info)) {
            //printf("EMPTY STACK\n");
            stack_t *tmp_stack = S_Init();
            p = (long*) my_get_host_by_name(dns_server_address, tmp_stack, tmp_stack, auth_server->RDATA, 1, 1, 1);
        }
        else {
            p = (long *) S_Top(add_info);
        }

        addr.sin_addr.s_addr = (*p);
        //last_auth_server = inet_ntoa(addr.sin_addr);
        //printf("IP: %s\n", inet_ntoa(addr.sin_addr));
        //S_Pop(add_info);
        add_info = S_Comeback(add_info);

        S_Pop(servers_queue);
        printf("%s %s %s %s\n", auth_server->RDATA, class, str_record_type, inet_ntoa(addr.sin_addr));
        tmp = my_get_host_by_name(inet_ntoa(addr.sin_addr), add_info, servers_queue, domain_name, record_type, 0, 0);
    }
}

int print_help() {

    printf("IPK-LOOKUP.\n");
    return 0;
}

void parse_arg(int arg_count, char **arg_array) {

    int opt;
    while ((opt = getopt(arg_count, arg_array, "hs:p:T:t:i")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                break;
            case 's':
                strcpy(dns_server_address, optarg);
                break;
            case 'p':
                if (0 <= atoi(optarg) && atoi(optarg) <= 65535)
                    port_number = atoi(optarg);
                break;
            case 'T':
                TIMEOUT = atoi(optarg);
                break;
            case 't':
                if (!strcmp(optarg, "AAAA")) record_type = 28;
                if (!strcmp(optarg, "NS")) record_type = 2;
                if (!strcmp(optarg, "PTR")) record_type = 12;
                if (!strcmp(optarg, "CNAME")) record_type = 5;
                break;
            case 'i':
                iterative = true;
                break;
            default:
                fprintf(stderr, "Usage: ./ipk-lookup [-h]\n./ipk-lookup -s server [-T timeout] [-t type] [-i] name");
                exit(EXIT_FAILURE);
        }
    }

    strcpy(domain_name, arg_array[arg_count - 1]);
}

int main(int argc, char **argv) {

    parse_arg(argc, argv);
    if (iterative)
        iterative_query();
    else {
        printf("NON ITERATIVE\n");
        stack_t *tmp_stack = S_Init();
        my_get_host_by_name(dns_server_address, tmp_stack, tmp_stack, domain_name, record_type, 0, 1);
    }

    return 0;
}