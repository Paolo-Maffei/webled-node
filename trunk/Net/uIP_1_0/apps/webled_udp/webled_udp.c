/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "webled_udp.h"
#include "data_pack.h"

char udp_send_buf[UDP_SBUF_SIZE];
char udp_send_len;

void WebLED_App_Init(void)
{
        uip_ipaddr_t addr;
	struct uip_udp_conn *conn = NULL;
        uip_ipaddr(addr,255,255,255,255);

	conn = uip_udp_new(&addr, HTONS(WEBLED_UDP_SEND_PORT));
	if(conn != NULL) {
		uip_udp_bind(conn, HTONS( WEBLED_UDP_RCV_PORT));
	}//收发采用同一个端口
}

void WebLED_UDP_APPCALL(void)
{
       switch(HTONS(uip_udp_conn->lport))
       {
        case WEBLED_UDP_RCV_PORT:
          if (uip_newdata())
          {
            udp_send_len = 0;
 //           u16_t len = uip_datalen();
            char *dataptr = (char *)uip_appdata;
            if((*(int *)(dataptr+1) != NodeAttr_GetID()) && 
               (0xffffffff != *(int *)(dataptr+1)) && (0x30!=(dataptr[0]&0xf0)))  
              break;        //目的ID和本节点ID不同,且不为广播ID,且不为场景命令
            switch(*dataptr){
            case 0x10:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              udp_send_len = 9;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x11:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              CopyMemory(&udp_send_buf[9],&node_info.type,1);              
              CopyMemory(&udp_send_buf[10],&node_info.status,5);
              CopyMemory(&udp_send_buf[15],&node_info.name_length,1);
              CopyMemory(&udp_send_buf[16],&node_info.name,node_info.name_length);            
              udp_send_len = 16 + node_info.name_length;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x12:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              udp_send_buf[9] = node_info.name_length;
              CopyMemory(&udp_send_buf[10],node_info.name,node_info.name_length);
              udp_send_len = node_info.name_length + 10;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x13:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              udp_send_buf[9] = Group_Match_Table.size;
              CopyMemory(&udp_send_buf[10],Group_Match_Table.table,
                         sizeof(*Group_Match_Table.table)*Group_Match_Table.size);
              udp_send_len = sizeof(*Group_Match_Table.table)*Group_Match_Table.size + 10;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x14:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              CopyMemory(&udp_send_buf[9],&node_info.type,sizeof(node_info.type));
              udp_send_len = sizeof(node_info.type) + 9;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x15:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              CopyMemory(&udp_send_buf[9],&node_info.status,sizeof(node_info.status));
              udp_send_len = sizeof(node_info.status) + 9;
              uip_send(udp_send_buf,udp_send_len);
              break;
            case 0x16:
              break;
              
            case 0x20:   //none    
              break;
            case 0x21:   //set ssid 
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              char ssid_length = dataptr[9];
              NodeAttr_SetSSID(&dataptr[10],ssid_length);              
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);              
              break;
            case 0x22:   //set name    
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              char name_length = dataptr[9];
              NodeAttr_SetName(&dataptr[10],name_length);
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);     
              break;
            case 0x23:  //set groupIDs
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              GroupTable_DelAll();   //删除所有已有场景
              char group_length = dataptr[9];
              for(int i=0;i<group_length;i++)  //循环依次添加场景
              {
                GroupTable_Add(GroupTable_IDasm(&dataptr[10+4*i]));
              }
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;
            case 0x24:    //add groupID
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              GroupTable_Add(GroupTable_IDasm(&dataptr[9]));
              NodeAttr_SetStatus();
              
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;
            case 0x25:      //delete groupID
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              GroupTable_Del(GroupTable_IDasm(&dataptr[9]));
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;
            case 0x26:    //set type
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&dataptr[1],4);
              NodeAttr_SetType(dataptr[9]);
              udp_send_buf[9] = 0;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;        
            case 0x27:    //clear ip and id's config
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&dataptr[1],4);
              NodeAttr_Clr_Config_Status();
              udp_send_buf[9] = 0;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;
              
              
              
            case 0x31:    //trigger groupID
              //此处添加响应函数
              if(GroupTable_Exist(GroupTable_IDasm(&dataptr[1])))
              {
                LED_TurnOn(LED1);
                LED_TurnOn(LED2);
              }else
                break;
              //
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[1],4);  //copy groupID
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              udp_send_len = 9;
              uip_send(udp_send_buf,udp_send_len);   
              break;
            case 0x32:    //close GroupID
               //此处添加响应函数
              if(GroupTable_Exist(GroupTable_IDasm(&dataptr[1])))
              {
                LED_TurnOff(LED1);
                LED_TurnOff(LED2);
              }else
                break;
              //
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[1],4);  //copy groupID
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              udp_send_len = 9;
              uip_send(udp_send_buf,udp_send_len);   
              break;
            case 0x33:    //close all GroupIDs
               //此处添加响应函数
                LED_TurnOff(LED1);
                LED_TurnOff(LED2);
              //              
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[1],4);  //copy groupID
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              udp_send_len = 9;
              uip_send(udp_send_buf,udp_send_len);  
              break;              
            
            default:
              break;
            }
          }
        }
}

void GetInfo_Server()
{
	
}

void SetInfo_Server()
{
	
}

void Ctrl_Server() //LED控制服务器
{
	
}

