/******************************************************************************
* 成都达创科技
* by lits @2013/01/22
*******************************************************************************/

#include "Project.h"
#include "uip.h"
#include "webled_udp.h"

char udp_send_buf[UDP_SBUF_SIZE];
char udp_send_len;

void WebLED_App_Init(void)
{
        uip_ipaddr_t addr;
	struct uip_udp_conn *conn1 = NULL;        
        uip_ipaddr(addr,255,255,255,255);

	conn1 = uip_udp_new(&addr, HTONS(WEBLED_UDP_SEND_PORT));
	if(conn1 != NULL) {
		uip_udp_bind(conn1, HTONS(WEBLED_UDP_RCV_PORT));
	}//收发采用同一个端口
        
#ifdef NODE_TYPE_PANEL
        struct uip_udp_conn *conn2 = NULL;
        uip_ipaddr(addr,255,255,255,255);
        conn2 = uip_udp_new(&addr, HTONS(PANEL_UDP_SEND_PORT));
#endif //NODE_TYPE_PANEL
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
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              ADC_GetValue(&udp_send_buf[9]);
              udp_send_len = 17;
              uip_send(udp_send_buf,udp_send_len);
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
            case 0x23:  //set group entries
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              GroupTable_DelAll();   //删除所有已有场景
              char group_length = dataptr[9];
              for(int i=0;i<group_length;i++)  //循环依次添加场景
              {
                GroupTable_Add(GroupTable_IDasm(&dataptr[10+9*i]),&dataptr[14+9*i]);
              }
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;
            case 0x24:    //add groupID
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              GroupTable_Add(GroupTable_IDasm(&dataptr[9]),&dataptr[13]);         
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
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              NodeAttr_SetType(dataptr[9]);
              udp_send_buf[9] = 0;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;        
            case 0x27:    //clear ip and id's config
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              NodeAttr_Clr_Config_Status();
              udp_send_buf[9] = 0;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);   
              break;
            case 0x28:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              LED_Update(&dataptr[9]);
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);     
              break;
#ifdef NODE_TYPE_PANEL          
            case 0x29:
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              NodeAttr_SetPanelKeyID(dataptr[9],GroupTable_IDasm(&dataptr[10]));
              udp_send_buf[9] = dataptr[9];
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);     
              break;
#endif  //NODE_TYPE_PANEL
            case 0x30:   //set ssid key 
              udp_send_buf[0] = dataptr[0]+0x80;
              CopyMemory(&udp_send_buf[1],&dataptr[5],4);
              CopyMemory(&udp_send_buf[5],&node_info.id,4);
              char key_length = dataptr[9];
              NodeAttr_SetKey(&dataptr[10],key_length);              
              udp_send_buf[9] = RESULT_SUCCESS;
              udp_send_len = 10;
              uip_send(udp_send_buf,udp_send_len);              
              break;        
              
            case 0x31:    //trigger groupID
              //
              char status[5];
              if( GroupTable_GetStatus(GroupTable_IDasm(&dataptr[1]),status) )
              {
                  LED_Update(status);
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
               //
              if(GroupTable_Exist(GroupTable_IDasm(&dataptr[1])))
              {
                char zero_status[5]={0,0,0,0,0};
                LED_Update(zero_status);
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
               //
              char zero_status[5]={0,0,0,0,0};
              LED_Update(zero_status);
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
#ifdef  NODE_TYPE_PANEL       
       switch(HTONS(uip_udp_conn->rport))
       {
       case PANEL_UDP_SEND_PORT:
         if(uip_poll())
         {
           for(char i=0;i<4;i++) //遍历所有按键
           {
             if( ((panel_status&(0x01<<i)) != (node_info.panel_status&(0x01<<i))) && (0xFFFFFFFF != node_info.panel_key[i]) )
             {
//               if(0 != (panel_status>>i)&0x01 )   //turn on 
//               {
                 //发送开启命令
                 udp_send_buf[0] = 0x41;
                 GroupTable_IDdeasm(node_info.panel_key[i],&udp_send_buf[1]);
                 CopyMemory(&udp_send_buf[5],&node_info.id,4);
                 udp_send_len = 9;
                 uip_send(udp_send_buf,udp_send_len); 
                 //更新并保存状态 
                 node_info.panel_status &= ~(0x01<<i);
                 node_info.panel_status |= (panel_status&(0x01<<i));
                 NodeAttr_SetPanelStatus(node_info.panel_status);
                 break;
//               }
//               else                      //turn off
//               {
//                 //发送关闭命令
//                 udp_send_buf[0] = 0x42;
//                 GroupTable_IDdeasm(node_info.panel_key[i],&udp_send_buf[1]);
//                 CopyMemory(&udp_send_buf[5],&node_info.id,4);
//                 udp_send_len = 9;
//                 uip_send(udp_send_buf,udp_send_len); 
//                 //更新并保存状态 
//                 node_info.panel_status &= ~(0x01<<i);
//                 node_info.panel_status |= (panel_status&(0x01<<i));
//                 NodeAttr_SetPanelStatus(node_info.panel_status);
//                 break;
//               }
             }
           }
         }
       }
#endif //NODE_TYPE_PANEL
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

