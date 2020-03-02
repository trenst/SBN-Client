#include <unistd.h>

#include "sbn_client.h"
#include "sbn_client_minders.h"
#include "sbn_client_utils.h"

extern int sbn_client_sockfd;

boolean continue_heartbeat = TRUE;
boolean continue_receive_check = TRUE;


/* Deal with sending out heartbeat messages */
void *SBN_Client_HeartbeatMinder(void *vargp)
{
    while(continue_heartbeat) // TODO: check run state?
    {
        if (sbn_client_sockfd != 0)
        {
            send_heartbeat(sbn_client_sockfd);
        }
        
        sleep(3);
    }
    
    return NULL;
}


void *SBN_Client_ReceiveMinder(void *vargp)
{
    int32 status;
    
    while(continue_receive_check) // TODO: check run state?
    {
        status = recv_msg(sbn_client_sockfd); // TODO: pass message pointer?
        // On heartbeats, need to update known liveness state of SBN
        // On other messages, need to make available for next CFE_SB_RcvMsg call
        
        if (status != CFE_SUCCESS)
        {
            char err_msg[50];
            
            snprintf(err_msg, 50, "Recieve message returned error 0x%08X\n", 
              status);
            
            log_message(err_msg);
        }/* end if */
        
    }
    
    return NULL;
}