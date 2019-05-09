#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "sbn_constants.h"
#include "sbn_interfaces.h"
#include "sbn_pack.h"

#define  DEFAULT_PROTOCOL   0

/**
 * Just trying to figure out how to connect to the tcp sbn module
 * It looks like the SBN header is just:
 *     uint16 MsgSz, uint8 MsgType, uint32 ProcessorID
 * The rest of the info is in the CCSDS header I guess... which may be built a few ways...
 *     TODO: get the CCSDS header
 * For now I can hard code all of this.
 */

#define CCSDS_TIME_SIZE 6 // <- see mps_mission_cfg.h

// Refer to sbn_cont_tbl.c to make sure these match
// SBN is running here:
#define PORT    1234
#define IP_ADDR "127.0.0.1"

// Peer (us) is running here:
#define PEER_PORT    1235
#define PEER_IP_ADDR "127.0.0.1"

// Private functions
int32 SBN_ClientInit(void);
int connect_to_server(const char *server_ip, uint16_t server_port);
int send_msg(int sockfd, CFE_SB_Msg_t *msg);
int send_heartbeat(int sockfd);
int recv_msg(int sockfd);
///////////////////////

int sockfd = 0;
int cpuId = 0;
struct sockaddr_in server_address;

void *heartbeatMinder(void *vargp);
pthread_t heart_thread_id;
void *receiveMinder(void *vargp);
pthread_t receive_thread_id;


int32 SBN_ClientInit(void)
{
    // Gets socket file descriptor
    sockfd = connect_to_server(IP_ADDR, PORT);
    cpuId = 2;
    
    if (sockfd < 0)
    {
      printf("SBN_clint failed to get sockfd, error %d\n", sockfd);
      exit(sockfd);
    }

    // Receive a message or something?
    recv_msg(sockfd);
    recv_msg(sockfd);

    // Thread for watchdog?
    pthread_create(&heart_thread_id, NULL, heartbeatMinder, NULL);
    pthread_create(&receive_thread_id, NULL, receiveMinder, NULL);

    // TODO: is thread ever cleaned up?
    // pthread_join(thread_id, NULL);

    // TODO: return failure?
    return OS_SUCCESS;
}

int connect_to_server(const char *server_ip, uint16_t server_port)
{
    int address_converted, connection;

    // Create an ipv4 TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);

    // socket error
    if (sockfd < 0)
    {
        return -1;
    }

    memset(&server_address, '0', sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    address_converted = inet_pton(AF_INET, server_ip, &server_address.sin_addr);

    // inet_pton can have two separate errors, a value of 1 is success.
    if (address_converted == 0)
    {
        return -2;
    }

    if (address_converted == -1)
    {
        return -3;
    }

    connection = connect(sockfd, (struct sockaddr *)&server_address,
                         sizeof(server_address));

    // connect error
    if (connection < 0)
    {
        return -4;
    }

    return sockfd;
}

// SBN header // TODO: from include?
typedef struct {
    uint16_t SBN_MsgSz;
    uint8_t  SBN_MsgType;
    uint32_t SBN_ProcessorID;
} SBN_Hdr_t;

// Deal with sending out heartbeat messages
#define SBN_TCP_HEARTBEAT_MSG 0xA0
void *heartbeatMinder(void *vargp)
{
    while(1) // TODO: check run state?
    {
        sleep(1);
        if (sockfd != 0)
        {
            printf("SBN_Client: Sending heartbeat\n");
            send_heartbeat(sockfd);
        }
    }
    return NULL;
}

// TODO: return value?
int send_heartbeat(int sockfd)
{
    printf("Sending a heartbeat\n");

    int retval = 0;
    char sbn_header[SBN_PACKED_HDR_SZ] = {0};

    Pack_t Pack;
    Pack_Init(&Pack, sbn_header, 0 + SBN_PACKED_HDR_SZ, 0);

    Pack_UInt16(&Pack, 0);
    Pack_UInt8(&Pack, SBN_TCP_HEARTBEAT_MSG);
    Pack_UInt32(&Pack, 2);

    retval = write(sockfd, sbn_header, sizeof(sbn_header));

    printf("Did the send work? %d\n", retval);
}


// Pipe creation / subscription

int32 __wrap_CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
  printf ("CFE_SB_CreatePipe not yet implemented\n");
  return -1;
}

int32 __wrap_CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
  printf ("CFE_SB_DeletePipe not yet implemented\n");
  return -1;
}

int32 __wrap_CFE_SB_Subscribe(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
  printf ("CFE_SB_Subscribe not yet implemented\n");
  return -1;
}

int32 __wrap_CFE_SB_SubscribeEx(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim)
{
  printf ("CFE_SB_SubscribeEx not yet implemented\n");
  return -1;
}

int32 __wrap_CFE_SB_SubscribeLocal(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId, uint16 MsgLim)
{
  printf ("CFE_SB_SubscribeLocal not yet implemented\n");
  return -1;
}

int32 __wrap_CFE_SB_Unsubscribe(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
  printf ("CFE_SB_Unsubscribe not yet implemented\n");
  return -1;
}

int32 __wrap_CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId)
{
  printf ("CFE_SB_UnsubscribeLocal not yet implemented\n");
  return -1;
}

uint32 __wrap_CFE_SB_SendMsg(CFE_SB_Msg_t *msg)
{
  printf("Sending from client...\n");
  char *buffer;
  uint16 msg_size = CFE_SB_GetTotalMsgLength(msg);
  
  size_t write_result, total_size = msg_size + SBN_PACKED_HDR_SZ;
  Pack_t Pack;
  
  if (total_size > CFE_SB_MAX_SB_MSG_SIZE)
  {
    return CFE_SB_MSG_TOO_BIG;
  }
  
  buffer = malloc(total_size);
  
  Pack_Init(&Pack, buffer, total_size, 0);

  Pack_UInt16(&Pack, msg_size);
  Pack_UInt8(&Pack, SBN_APP_MSG);
  Pack_UInt32(&Pack, cpuId); 
  
  memcpy(buffer + SBN_PACKED_HDR_SZ, msg, msg_size);
  
  write_result = write(sockfd, buffer, total_size);
  
  if (write_result != total_size)
  {
    return CFE_SB_BUF_ALOC_ERR;
  }
  
  free(buffer);
  
  return CFE_SUCCESS;
}

int32 __wrap_CFE_SB_RcvMsg(CFE_SB_MsgPtr_t *BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    // Oh my.
    // Need to have multiple pipes... so the subscribe thing
    // Need to coordinate with the recv_msg thread... so locking?
    // Also, what about messages that get split? Is that an issue?
}

int32 __wrap_CFE_SB_ZeroCopySend(CFE_SB_Msg_t *MsgPtr, CFE_SB_ZeroCopyHandle_t BufferHandle)
{
  printf ("CFE_SB_ZeroCopySend not yet implemented\n");
  return -1;
}
// Receiving messages

void *receiveMinder(void *vargp)
{
    while(1) // TODO: check run state?
    {
        printf("SBN_Client: Checking messages\n");
        recv_msg(sockfd); // TODO: pass message pointer?
        // On heartbeats, need to update known liveness state of SBN
        // On other messages, need to make available for next CFE_SB_RcvMsg call
    }
}

int recv_msg(int sockfd)
{
    // CFE_SB_Msg_t *msg;
    int retval = 0;
    char buffer[CFE_SB_MAX_SB_MSG_SIZE]; // TODO: Plus SBN header?

    SBN_MsgSz_t MsgSz;
    SBN_MsgType_t MsgType;
    SBN_CpuID_t CpuID;

    retval = read(sockfd, buffer, sizeof(buffer));
    printf("Received: %d\t", retval);

    // TODO: error checking (-1 returned, perror)

    Unpack_t Unpack;
    Unpack_Init(&Unpack, buffer, SBN_MAX_PACKED_MSG_SZ);
    Unpack_UInt16(&Unpack, &MsgSz);
    Unpack_UInt8(&Unpack, &MsgType);
    Unpack_UInt32(&Unpack, &CpuID);

    for (int i = 0; i < retval; i++)
    {
        printf("0x%02X ", (unsigned char)(buffer[i]));
    }
    printf("\n");

    printf("Msg Size: %d\t Msg Type: %d\t Processor_ID: %d\n", MsgSz, MsgType, CpuID);
}