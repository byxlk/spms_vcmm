
#include "iec101.h"
#include "TypeRAM.h"
#include "data.h"
#define justinit 1
#define initend 2
#define notinit 0

#define yxbw   1
#define substinit   2
#define readcmd   3
#define substchange 4
#define nofirstdata 5 

#define FIXDMSG  "LINELOSS/FIXD/fixd01.msg"
#define FIXDXML  "LINELOSS/FIXD/fixd01.xml"
#define FRZDMSG  "LINELOSS/FRZD/frzd01.msg"
#define FRZDXML  "LINELOSS/FRZD/frzd01.xml"
#define SHARPMSG "LINELOSS/SHARPD/sharpd01.msg"
#define SHARPXML "LINELOSS/SHARPD/sharpd01.xml"
#define MONMSG   "LINELOSS/MONTHD/monthd01.msg"
#define MONXML   "LINELOSS/MONTHD/monthd01.xml"
#define EVTMSG   "LINELOSS/EVENTD/eventd01.msg"
#define EVTXML   "LINELOSS/EVENTD/eventd01.xml"


typedef struct {
  u16 info_addr;
  u16 data_len;
  unsigned char *info_data;
}S_SNINFO;
unsigned char s_Devtype[]="FTU";
unsigned char s_Operation[]="N/A";
unsigned char s_Manufacture[]="�滪";
unsigned char s_Hardwarever[]="A";
unsigned char s_Firmwarever[]="00.01";
unsigned char s_FirmwareCrc[]="0x1234";
unsigned char s_Protocolver[]="V1.000";
unsigned char s_Model[]="JH4000";
unsigned char s_Id[]="201710280001";
S_SNINFO s_sninfo[]=
{
  0x8001,3,s_Devtype,
  0x8002,3,s_Operation,
  0x8003,4,s_Manufacture,
  0x8004,1,s_Hardwarever,
  0x8005,5,s_Firmwarever,
  0x8006,6,s_FirmwareCrc,
  0x8007,6,s_Protocolver,
  0x8008,6,s_Model,
  0x8009,12,s_Id,
};


unsigned char *File_List[]={FIXDMSG,FIXDXML,FRZDMSG,FRZDXML,SHARPMSG,SHARPXML,MONMSG,MONXML,EVTMSG,EVTXML};

static struct IEC101_STRUCT m_IEC101;
struct IEC101_STRUCT *lpIEC101=&m_IEC101;	//IEC101��Լ˽������ָ��

unsigned long GetYc(unsigned short ycno)
{
  unsigned long *Ptr;
  Ptr=(unsigned long*)&Real_Data[ycno/31];
  return Ptr[ycno%31];
}
short GetDd(unsigned short kwhno,unsigned char *buf)
{
  int m_ch,ptr_v;
  unsigned long *Ptr;
  unsigned long m_Lval;
  unsigned char tmp_buf[48];
  float f_val;
  m_ch=kwhno/16;
  ptr_v=kwhno%16;
  if(ptr_v<8)
  {
   // Ptr=(unsigned long*)&Energy_Data[m_ch];
    //memcpy(buf,&Ptr[ptr_v],4);
    Ptr=(unsigned long*)&Energy_Data[m_ch];
    f_val = Ptr[ptr_v]; 
    f_val = f_val/1000;
    memcpy(buf,(unsigned char *)&f_val,4);
    //*(buf) = tmp_buf[0];//(u8)nVal;	// ң��ֵ
//		*(lpby + byMsgNum ++) = tmp_buf[1];//(u8)(nVal >> 8);
  //                      *(lpby + byMsgNum ++) = tmp_buf[2];//(u8)(nVal >> 16);
    //                    *(lpby + byMsgNum ++) = tmp_buf[3];//(u8)(nVal >> 24);
    return 4;
  }
  else
  {
    Read_LastData(m_ch,tmp_buf);
    memset(buf,0,12);
   // memcpy(buf,tmp_buf+6+4*(ptr_v-8),4);
    //Ptr = (unsigned long*)tmp_buf[6+4*(ptr_v-8)];
    //f_val = Ptr[0];
    memcpy(&m_Lval,tmp_buf+6+4*(ptr_v-8),4);
    f_val = m_Lval;
    f_val = f_val/1000;
    memcpy(buf,(unsigned char *)&f_val,4);
    memcpy(buf+6,tmp_buf,6);
    return 12;
  }
}
//���ܣ����մ�����
//���룺void
//�����void
void IEC101RErrorProcess(void)
{
	lpIEC101->wRecvNum = 0;
	lpIEC101->RErrorN++;	   	//��Լ���մ�������
	if(lpIEC101->RErrorN>=MaxErrorN)
	{
		lpIEC101->RErrorN=0;	   //��Լ���մ�������
		if((lpIEC101->initstatus == initend) &&(lpIEC101->FtuSccReset == FALSE))
	   	{
	    		//fnInitSCC(ChanlNo,SCCERROR); //��λͨ��
	   		lpIEC101->FtuSccReset = TRUE; //��վ״̬�ı�
		}
	}
}
//��·����պ���
//���ܣ���·���գ��Ѵ��ڽ��ջ������е����ݽ��һ��
//      ������lpIEC101->PRecvFrame.byLinkBuf
//      ȡ����·���ַ
//���룺void
//�����void

void Iec101LinkRecv(void)
{
  u8 Count,byRecv,bySum = 0;
  int i,j;
  Count=Serial_Read(2,lpIEC101->byRecvBuf+lpIEC101->wRecvNum,128);
  if(Count)
  {
    lpIEC101->wRecvNum +=Count;
    for(i=0;i<lpIEC101->wRecvNum;++i)
    {  
      if(lpIEC101->byRecvBuf[i]!=F_STARTCODE && lpIEC101->byRecvBuf[i]!=V_STARTCODE)
      {
        continue;
      }
      if(lpIEC101->byRecvBuf[i]==F_STARTCODE)
      {
        if(lpIEC101->wRecvNum-i>=5 && (lpIEC101->byRecvBuf[lpIEC101->wRecvNum-i-1]==0x16))
        {  
          memcpy(lpIEC101->PRecvFrame.byLinkBuf,lpIEC101->byRecvBuf+i,lpIEC101->wRecvNum-i);
          lpIEC101->wRecvCmmAdd.Byte.l = lpIEC101->PRecvFrame.byLinkBuf[2];
          if(lpIEC101->TypeLinkAdd==2)
          {
            lpIEC101->wRecvCmmAdd.Byte.h = lpIEC101->PRecvFrame.byLinkBuf[3];
          }
          lpIEC101->wRecvNum=0;
          lpIEC101->PRecvFrame.byFull=1;
        }
        break;
      }
      if(lpIEC101->byRecvBuf[i]==V_STARTCODE)
      {
        if((lpIEC101->byRecvBuf[i]==lpIEC101->byRecvBuf[i+3])
					&& (lpIEC101->byRecvBuf[i+1]==lpIEC101->byRecvBuf[i+2]))
        {
          lpIEC101->wRecvLen = lpIEC101->byRecvBuf[i+1] + 6;
          if((lpIEC101->wRecvNum-i)>=lpIEC101->wRecvLen)
          {
            //Serial_Write(2,"Long Frame\r\n",12);
            memcpy(lpIEC101->PRecvFrame.byLinkBuf,lpIEC101->byRecvBuf+i,lpIEC101->wRecvNum-i);
            lpIEC101->wRecvCmmAdd.Byte.l = lpIEC101->PRecvFrame.byLinkBuf[5];
            if(lpIEC101->TypeLinkAdd==2)
            {
              lpIEC101->wRecvCmmAdd.Byte.h = lpIEC101->PRecvFrame.byLinkBuf[6];
            }
            lpIEC101->wRecvNum=0;
            lpIEC101->PRecvFrame.byFull=1;
          }
          break;
        }
      }
    }
  }
}
//�Ӷ���·����մ�������
//���ܣ�������·������ִ����Ӧ�Ĳ�����
//      1�����ݹ�����ִ�ж���
//      2��FCBλ�жϺ�����
//���룺��·������
//�����void

void PLinkRecvProcessF(u8 byConField)
{
  lpIEC101->PRecvFrame.byFunCode = byConField & 0x0F;
  if (lpIEC101->initstatus == notinit)	    
  {
    if ( (lpIEC101->PRecvFrame.byFunCode == CALL_LINK)
              ||(lpIEC101->PRecvFrame.byFunCode == RESET_LINK) )
    {
      if (lpIEC101->PRecvFrame.byFunCode == CALL_LINK)
      {
        lpIEC101->PSeAppLayer.LinkFunCode = 0x0B;
        if (lpIEC101->haveset == TRUE)
        {
          lpIEC101->initstatus = justinit;
        }
      }		
      else if (lpIEC101->PRecvFrame.byFunCode == RESET_LINK)
      {
        lpIEC101->PfcbC = 0;
        lpIEC101->PSeAppLayer.LinkFunCode = 0x00;
	lpIEC101->haveset = TRUE;
        lpIEC101->initstatus = justinit;
        lpIEC101->PWinTimer=0;
      }
      lpIEC101->PSeAppLayer.byMsgNum = 0;
      lpIEC101->PSeAppLayer.byFull = 1;
    }
    return;
  }
  
  if ( ((byConField & 0x20)==(lpIEC101->PfcbC<<5)) && (byConField & 0x10))
  {
    lpIEC101->PSendFrame.byFull = 1;
    return;
  }
  
  if((byConField & 0x40)==0)
  {
    switch(lpIEC101->PRecvFrame.byFunCode)
    {
    case RESET_LINK:
      if (lpIEC101->initstatus == justinit)
        {
          lpIEC101->PReFrameType = CALL_DATA1;
          lpIEC101->firstData = substinit;		// ��վ��ʼ��
          lpIEC101->wCmmAdd.Byte.l = 1;
          lpIEC101->wCmmAdd.Byte.h = 0;
          lpIEC101->byCOI = 2;
          lpIEC101->Pacd = 3;
          lpIEC101->Pdfc = 1;
          lpIEC101->PSendFrame.byFunCode = 3;
	}
       // else
     //   {
      //    lpIEC101->PReFrameType = 0x3;
      //    lpIEC101->PSendFrame.byFunCode = 3;
     //   }
      
      break;
    case LINK_GOOD:
      lpIEC101->PReFrameType = lpIEC101->PRecvFrame.byFunCode;
      lpIEC101->PSeAppLayer.LinkFunCode = 0x00;
      lpIEC101->Pacd = 2;
    //  lpIEC101->PSendFrame.byFull=1;
      break;
    default:
      lpIEC101->PSendFrame.byFull=0;
      break;
    }
    
    return;
  }
 
  if ( (lpIEC101->PRecvFrame.byFunCode == CALL_LINK) 
		||(lpIEC101->PRecvFrame.byFunCode == RESET_LINK)
		||(lpIEC101->PRecvFrame.byFunCode == CALL_ACD) )
  {
    if (lpIEC101->PRecvFrame.byFunCode == RESET_LINK)
    {
      lpIEC101->PfcbC = 0;
      lpIEC101->PSeAppLayer.LinkFunCode = 0x00;
      lpIEC101->initstatus = justinit;
      lpIEC101->PWinTimer=0;
    }
    if (lpIEC101->PRecvFrame.byFunCode == CALL_LINK)
    {
      lpIEC101->PSeAppLayer.LinkFunCode = 0x0B;
      if (lpIEC101->haveset == TRUE)
        {
          lpIEC101->initstatus = justinit;
        }
    }
    if (lpIEC101->PRecvFrame.byFunCode == CALL_ACD)
      lpIEC101->PSeAppLayer.LinkFunCode = 0x0B;
		
    lpIEC101->PSeAppLayer.byMsgNum = 0;
    lpIEC101->PSeAppLayer.byFull = 1;
  }
  
  if ((lpIEC101->PRecvFrame.byFunCode == CALL_DATA1)||(lpIEC101->PRecvFrame.byFunCode == CALL_DATA2))
  {
    lpIEC101->PReFrameType = lpIEC101->PRecvFrame.byFunCode;
    lpIEC101->PfcbC = byConField & 0x20 ? 1 : 0;	// FCVλ��Ч,�򱣴�FCBλ
  }       
}
//�Ӷ�Ӧ�ò���մ�������
//���ܣ�������·�����ּ����ͱ�ʶ����Ӧ�Ĵ���
//���룺void
//�����void

void PLinkRecvProcessV(u8 byConField)
{
  u8 byFrameCount,i; 
#if 0       
  if ( ((byConField & 0x20)==(lpIEC101->PfcbC<<5)) && (byConField & 0x10))
  {
    lpIEC101->PSendFrame.byFull = 1;
    return;
  }
#endif       
  lpIEC101->PReAppLayer.lpByBuf = lpIEC101->PRecvFrame.byLinkBuf;
  lpIEC101->PRecvFrame.byFunCode = byConField & 0x0F;
  if(3==lpIEC101->PRecvFrame.byFunCode)
  {
    if(3==lpIEC101->UnsolTimeInterval)    //�ȷ���·ȷ���ٵȴ��ٻ���������
    {
      lpIEC101->PReFrameType = 3;
      lpIEC101->PSeAppLayer.LinkFunCode = 0x00;
      lpIEC101->PSeAppLayer.byMsgNum = 0;
      lpIEC101->PSeAppLayer.byFull = 1;
    }
  }
  lpIEC101->PfcbC = byConField & 0x20 ? 1 : 0;	// FCVλ��Ч,�򱣴�FCBλ
  byFrameCount=5+lpIEC101->TypeLinkAdd; //ָ�����ͱ�ʶ
  lpIEC101->PReMsgType = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];	// ���ͱ�ʶ
  lpIEC101->byQualify = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];		// �ɱ�ṹ�޶���
  lpIEC101->byReason  = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];		// ����ԭ��
  if(lpIEC101->TypeSeReason==2)
    lpIEC101->bySourceAdd = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];  //����ԭ��Ϊ�����ֽ�ʱȡ��Դ��ַ fu 2005.9.1
  lpIEC101->wCmmAdd.Byte.l = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];		// Ӧ�÷������ݵ�Ԫ�Ĺ�����ַ
  if(lpIEC101->TypeCmmAdd==2)
    lpIEC101->wCmmAdd.Byte.h = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];		// Ӧ�÷������ݵ�Ԫ�Ĺ�����ַ
  lpIEC101->dwInfAdd.Dword = 0;
  for(i=0;i<lpIEC101->TypeInfAdd;i++)
    lpIEC101->dwInfAdd.Byte[i] =  lpIEC101->PReAppLayer.lpByBuf[byFrameCount++]; 
  lpIEC101->byPSGenStep = 0;
  lpIEC101->byPSDdStep = 0;
  switch(lpIEC101->PReMsgType)
  {
  case GeneralCall: //���ٻ�
	lpIEC101->byQOI = lpIEC101->PReAppLayer.lpByBuf[byFrameCount];//[lpIEC101->PRecvFrame.wFrameLen-3]; //ָ��QOI
        if ( (lpIEC101->byReason == ACT) || (lpIEC101->byReason == DEACT) )
          lpIEC101->byPSGenStep = 0;
        else if (lpIEC101->byReason == REQ)
          lpIEC101->byPSGenStep = lpIEC101->byQOI - 20;
    //  lpIEC101->PReFrameType = CALL_DATA2; //zzl ����������վ
  //    lpIEC101->PSeAppLayer.byFull=0;
        lpIEC101->frameno = 0;
        break;
  case C_CS_NA_1:  //Уʱ
	{
                if(lpIEC101->byReason == ACT)
                  SettimeToCan(lpIEC101->PReAppLayer.lpByBuf+byFrameCount); //fulianqiang 2005.9.12,��ʱ����������ʹ�ó�����Ѱַ
	}
        break;
  case C_CI_NA_1: //�ٻ����
	{
          lpIEC101->byQCC = lpIEC101->PReAppLayer.lpByBuf[byFrameCount];//[lpIEC101->PRecvFrame.wFrameLen-3];
	//lpIEC101->byPSDdStep = (lpIEC101->byQCC & 0x3F);  //ȡʵ�ʽ��յ���ֵ��ԭ����ȥһ��
          break;
	}
  case C_RD_NA_1: //������
	{
		lpIEC101->dwReadAd.Dword = lpIEC101->dwInfAdd.Dword;
	}
        break;
  case C_RC_NA_1:
  case C_DC_NA_1:
  case C_SC_NA_1: //����˫��ң��	
	{
		lpIEC101->byDCO = lpIEC101->PReAppLayer.lpByBuf[byFrameCount];//[lpIEC101->PRecvFrame.wFrameLen-3];
		lpIEC101->WaitYkXzRet = 0;
		lpIEC101->WaitYkZxRet = 0;
		lpIEC101->SendYkZxAck = 0;
	}
        break;
  case C_TS_NA_1:  //����
	{
		lpIEC101->wTester.Byte.l =lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];
		lpIEC101->wTester.Byte.h =lpIEC101->PReAppLayer.lpByBuf[byFrameCount]; 
	}
        break;
  case C_RR_NA_1:
    break;
  case C_RS_NA_1:
          byFrameCount -= lpIEC101->TypeInfAdd;
          lpIEC101->Sn = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];
          lpIEC101->Sn = (lpIEC101->Sn) | (lpIEC101->PReAppLayer.lpByBuf[byFrameCount++]<<8);
          if(lpIEC101->byQualify) 
          {
            for(i=0;i<lpIEC101->byQualify;++i)
            {
              lpIEC101->List_para[i]=lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];
              lpIEC101->List_para[i]=(lpIEC101->List_para[i]) | (lpIEC101->PReAppLayer.lpByBuf[byFrameCount++]<<8);
            }
          }
          else //��ֵ��������в���
          {
          }
    break;
  case F_FR_NA_1:
    if(lpIEC101->PReAppLayer.lpByBuf[byFrameCount++]==0x02) //�������ݰ�����
    {
      lpIEC101->Fop = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];
      if(lpIEC101->Fop!=3)
      {  
        memcpy(&(lpIEC101->FId),lpIEC101->PReAppLayer.lpByBuf+byFrameCount,4);
        byFrameCount += 4;
      }
      memset(lpIEC101->Fname,0,32);
      if(lpIEC101->PReAppLayer.lpByBuf[byFrameCount])
      {
        memcpy(lpIEC101->Fname,lpIEC101->PReAppLayer.lpByBuf+byFrameCount+1,lpIEC101->PReAppLayer.lpByBuf[byFrameCount]);
       // byFrameCount += lpIEC101->PReAppLayer.lpByBuf[byFrameCount];
      }
      byFrameCount += lpIEC101->PReAppLayer.lpByBuf[byFrameCount]+1;
      lpIEC101->byDCO = lpIEC101->PReAppLayer.lpByBuf[byFrameCount++];
    }
    else
    {
      lpIEC101->PReMsgType = 0;
    }
    break;
  default:
    {
      lpIEC101->PReMsgType = 0;
    }
    break;
  }
}

//��·����մ�������
//���ܣ�ȡ����·�����֣����ô�������
//���룺void
//�����void

void Iec101LinkRecvPro(void)
{
  u8 byConField;
  if (lpIEC101->PRecvFrame.byFull == 1)
  {
    lpIEC101->Pacd = 0;
    lpIEC101->Pdfc = 0;
    if(lpIEC101->PRecvFrame.byLinkBuf[0] == F_STARTCODE)
    {
      if(lpIEC101->TypeLinkAdd==2)
      {
	       if( (lpIEC101->wRecvCmmAdd.Word == lpIEC101->wLinkAdd.Word)||(lpIEC101->wRecvCmmAdd.Word ==0xFFFF))
	        {   
	           byConField = lpIEC101->PRecvFrame.byLinkBuf[1];
	           PLinkRecvProcessF(byConField);
		   lpIEC101->OrgnizeFrame = 1 ;
		}
		else
		   lpIEC101->OrgnizeFrame = 0 ;
	    }
	    else
	    {
	       if( (lpIEC101->wRecvCmmAdd.Byte.l == lpIEC101->wLinkAdd.Byte.l)||(lpIEC101->wRecvCmmAdd.Byte.l ==0xFF))
 		{
		   byConField = lpIEC101->PRecvFrame.byLinkBuf[1];
 		   PLinkRecvProcessF(byConField);
		   lpIEC101->OrgnizeFrame = 1 ;
		}
		else
 		   lpIEC101->OrgnizeFrame = 0 ;
	    }
	 }
	 else if(lpIEC101->PRecvFrame.byLinkBuf[0] == V_STARTCODE)
	 {
	    if(lpIEC101->TypeLinkAdd==2)
	    {
	       if( (lpIEC101->wRecvCmmAdd.Word == lpIEC101->wLinkAdd.Word)||(lpIEC101->wRecvCmmAdd.Word ==0xFFFF))
	        {   
	           byConField = lpIEC101->PRecvFrame.byLinkBuf[4];
	           PLinkRecvProcessV(byConField);
		   lpIEC101->OrgnizeFrame = 1;
		}
		else
		   lpIEC101->OrgnizeFrame = 0 ;
	    }
	    else
	    {
	       if( (lpIEC101->wRecvCmmAdd.Byte.l == lpIEC101->wLinkAdd.Byte.l)||(lpIEC101->wRecvCmmAdd.Byte.l ==0xFF))
 		{
		   byConField = lpIEC101->PRecvFrame.byLinkBuf[4];
 		   PLinkRecvProcessV(byConField);
		   lpIEC101->OrgnizeFrame =1;
		}
		else
		   lpIEC101->OrgnizeFrame = 0;
	    }
	}
	lpIEC101->PRecvFrame.byFull=0;
	if((lpIEC101->FtuSccReset ==TRUE)&&(lpIEC101->OrgnizeFrame == 1))
	{
		lpIEC101->FtuSccReset = FALSE;	  //ͨ���ָ�����
	}
    }   
}
//��CAN��Уʱ
void SettimeToCan(u8* pbyBuf)
{ 
#if 0
        unsigned short wTotalMs;
	wTotalMs = (unsigned short)pbyBuf[1];
	wTotalMs = pbyBuf[0]|(wTotalMs<<8);
	sClock.wMS = wTotalMs % 1000;
	sClock.bySecond = (u8)(wTotalMs / 1000);
	//Уʱ֡�ķ����ֽ����λʱ����Ч��־δ��
	sClock.byMinute = pbyBuf[2] & 0x3F;
	sClock.byHour = pbyBuf[3] & 0x1F;
	sClock.byDay = pbyBuf[4] & 0x1F; 
	lpIEC101->byDayofWeek  = (pbyBuf[4] & 0xE0) >> 5;
	sClock.byMonth = pbyBuf[5] & 0x0F;
	sClock.wYear = (pbyBuf[6] & 0x7F)+2000;     
#else
        unsigned char pRTCTime[8];
        unsigned short wTotalMs;
	wTotalMs = (unsigned short)pbyBuf[1];
	wTotalMs = pbyBuf[0]|(wTotalMs<<8);
        pRTCTime[0] = (pbyBuf[4] & 0xE0) >> 5; //week
        pRTCTime[1] = (pbyBuf[6] & 0x7F); //year
        pRTCTime[2] = pbyBuf[5] & 0x0F;//month
        pRTCTime[3] = pbyBuf[4] & 0x1F; //day
        pRTCTime[4] = pbyBuf[3] & 0x1F;
        pRTCTime[5] = pbyBuf[2] & 0x3F;
        pRTCTime[6] = (u8)(wTotalMs / 1000);
        HT_RTC_Write(pRTCTime);
#endif        
}
//�Ӷ�Ӧ�ò���մ�������
//���ܣ�������·�����ּ����ͱ�ʶ����Ӧ�Ĵ���
//���룺void
//�����void

void SleaveIec101Frame(void)
{
	//��·����պ���
	Iec101LinkRecv();
	//��·����մ�������
	Iec101LinkRecvPro();
}
//���ӴӶ�����
void WatchPWindow(void)
{ 
	if (lpIEC101->PReMsgType || lpIEC101->PReFrameType
		|| lpIEC101->PSeAppLayer.byFull || lpIEC101->PSendFrame.byFull)
	{
		lpIEC101->PWindow = 1;
	}
        if(((lpIEC101->PRecvFrame.byFunCode == RESET_LINK) && lpIEC101->FlagPingH && (lpIEC101->initstatus == justinit)) || lpIEC101->wTester.Byte.l)
        {
                lpIEC101->PWindow = 1;
        }
}
//����һ������
void SearchFirstData(void)
{     
    	short wYxChang,wSendNo;
    	if(lpIEC101->FlagPingH)	  //�����ƽ�ⷽʽ
    	{
          if((lpIEC101->PRecvFrame.byFunCode == RESET_LINK) && (lpIEC101->PWinTimer>200) && (lpIEC101->initstatus == justinit))
          {
            lpIEC101->PWinTimer = 0;
            lpIEC101->PSeAppLayer.byFull = 1;
            lpIEC101->PSeAppLayer.LinkFunCode = CALL_LINK;
            lpIEC101->Pacd = 2;
          }
          if(lpIEC101->PRecvFrame.byFunCode == LINK_GOOD)
          {
            lpIEC101->PSeAppLayer.byFull = 1;
            lpIEC101->PSeAppLayer.LinkFunCode = RESET_LINK;
            lpIEC101->PRecvFrame.byFunCode = 0;
            lpIEC101->Pacd = 2;
          }
        /*  if(lpIEC101->wTester.Byte.l && lpIEC101->PWinTimer>200)
          {
            lpIEC101->PWinTimer = 0;
            lpIEC101->PSeAppLayer.LinkFunCode=TRAN_CONFIRM_DATA;
            lpIEC101->PSendFrame.byFunCode = TRAN_CONFIRM_DATA;
            lpIEC101->PReMsgType = C_TS_NA_1; 
          }*/
          return;
        }
	if (lpIEC101->firstData != nofirstdata)	// ����1���û�����,��ͣ����
   		return;
	if (lpIEC101->initstatus == justinit)
	{
		lpIEC101->firstData = substinit;		// ��վ��ʼ��
	}
#if 0       
	else if((lpIEC101->YxChangeC != sHostSoe.bySoeWrite)&&(initend==lpIEC101->initstatus))
	{
		wYxChang=lpIEC101->YxChangeC ;
		while (wYxChang != sHostSoe.bySoeWrite)
		{
			//����SOE��Ӧ��ң�ű�λ
			wSendNo = sHostSoe.SoeQueue[wYxChang].byYXSendSN;
			//�����ж�Ŀ���ǰ��շ��ͱ���ң�ű�λfulianqiang 2006.1.10
			if((wSendNo <= lpIEC101->SendYxN)&&(pYXSendTab[wSendNo].byUnitType==FTU))    //�жϷ���˳������Ƿ��д�ң��,�����ң�ű�λ
			{
				lpIEC101->firstData = yxbw;		// ң�ű�λ
				lpIEC101->YxChangeC= wYxChang;
				break;
			}
			if((++wYxChang) >= MAX_SOE_NUM )   //�ƶ�soe��ָ��
			      	wYxChang = 0;
		 }
	}
#endif        
    	if(nofirstdata==lpIEC101->firstData)
	{
		if (lpIEC101->dwReadAd.Dword)
	    	{
	   		lpIEC101->firstData = readcmd;		// �ɶ���������Ѱַ����Ϣ�������
	   	}
	}
	if (lpIEC101->firstData != nofirstdata)	// ������1���û�����
	{
		lpIEC101->Pacd = 1;			// ACD��λ
	}
	else
		lpIEC101->Pacd=0;        
}
//��ң�ű�λ��Ϣ��
u8 OrgnizeYxbwMsg(u8 *lpby)
{
#if 0 
        WORD wSendNo;
	FOUR_BYTE_TO_DWORD  dwInfoAd;
	u8 i,byMsgNum = 0;
	u8 * lpInfoNum;		   	// ��Ϣ����Ŀ��ַ û��Ҫ�Ӹ�ָ��fulianqiang 2005.9.2
	*(lpby + byMsgNum ++) = 1;   //���ͱ�ʶ	
	lpInfoNum = lpby + byMsgNum ++;	   //�ṹ�޶���lpby+1��Ӧ�ĵ�ַ
	*lpInfoNum = 0;
	*(lpby + byMsgNum ++) = 3;  //����ԭ��
	if(lpIEC101->TypeSeReason==2)		 //�Ƿ������ֽڵĴ���ԭ��
	   *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;		
       	*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
        if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
	//���SOE���е�ң�ű�λ������
	while (lpIEC101->YxChangeC != sHostSoe.bySoeWrite)
	{
		//����SOE��Ӧ��ң�ű�λ
		wSendNo = sHostSoe.SoeQueue[lpIEC101->YxChangeC].byYXSendSN;
		//�����ж�Ŀ���ǰ��շ��ͱ���ң�ű�λfulianqiang 2006.1.10
		if(wSendNo <= lpIEC101->SendYxN)		//�жϷ���˳������Ƿ��д�ң��
		{
                	dwInfoAd.Dword = wSendNo + IEC101_YXSA;
			dwInfoAd.Word[1] = lpIEC101->dwInfAdd.Word[1];
	        	for(i=0;i<lpIEC101->TypeInfAdd;i++)		//��Ϣ���ַ
	           		*(lpby + byMsgNum ++) = dwInfoAd.Byte[i];
			if (sHostSoe.SoeQueue[lpIEC101->YxChangeC].byOnOff == 0x10)
				*(lpby + byMsgNum ++) = 0;	
			else
				*(lpby + byMsgNum ++) = 1;
			if( ++(*lpInfoNum) >= IEC101_YXBWNPF)
				break;
		}
		if((++lpIEC101->YxChangeC) >= MAX_SOE_NUM )   //�ƶ�soe��ָ��
		      	lpIEC101->YxChangeC = 0;

	}
	return byMsgNum;
#else
        return 0;
#endif        
}
//���ʼ��������Ϣ��
u8 OrgnizeInitEndMsg(u8* pbyMsg)
{
 
	u8 i,byMsgNum = 0;         
	*(pbyMsg + byMsgNum++) = M_EI_NA_1;   //���ͱ�ʶ
	*(pbyMsg + byMsgNum++) = 1;
	*(pbyMsg + byMsgNum++) = INIT;
	if(lpIEC101->TypeSeReason==2)		 //�Ƿ������ֽڵĴ���ԭ��
	   *(pbyMsg + byMsgNum ++) = lpIEC101->bySourceAdd;		
       	*(pbyMsg + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
        if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           *(pbyMsg + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
        for(i=0;i<lpIEC101->TypeInfAdd;i++)		//��Ϣ���ַ fulianqiang 2005.9.2
              *(pbyMsg + byMsgNum ++) = 0; 
	*(pbyMsg + byMsgNum++) = lpIEC101->byCOI;        
	return byMsgNum;
}
//���������Ϣ��
u8 OrgnizeReadDataMsg(u8* lpby)
{
	int wYxTNo;
	u8 byYxVal,i;
	int wYcTNo;
	int  iYcVal;
	int wDdTNo;
	int dwDdVal;
	u8 byMsgNum = 1;
#if 1       
	*(lpby + byMsgNum ++) = 1;	// �ṹ�޶���
	*(lpby + byMsgNum ++) = SPONT;	//ͻ��
	if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
           *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;      //fulianqiang 2005.9.2
       	*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
        if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
        for(i=0;i<lpIEC101->TypeInfAdd;i++)
	    *(lpby + byMsgNum ++) = lpIEC101->dwReadAd.Byte[i];
	if (lpIEC101->TypeProtocol)	//2002��101��Լ
	{
#if 0          
		if ( (lpIEC101->dwReadAd.Word[0] >= IEC101_YXSA) && (lpIEC101->dwReadAd.Word[0] <= IEC101_YXEA_2002) )//fulianqiang 2005.9.2
		{
			*(lpby + 0) = 1;	 //���ͱ�ʶ
			wYxTNo = lpIEC101->dwReadAd.Word[0] - 1;
			byYxVal = GetYx(wYxTNo,YX_BIT);
			if (byYxVal)
				*(lpby + byMsgNum ++) = 1;
			else
				*(lpby + byMsgNum ++) = 0;
		}
		else
#endif                  
                if ( (lpIEC101->dwReadAd.Word[0] >= IEC101_YCSA_2002) && (lpIEC101->dwReadAd.Word[0] <= IEC101_YCEA_2002) )//fulianqiang 2005.9.2
		{
			*(lpby + 0) = 21;
			wYcTNo = lpIEC101->dwReadAd.Word[0] - IEC101_YCSA_2002;
			iYcVal = GetYc(wYcTNo);
			*(lpby + byMsgNum ++) = (u8)iYcVal;
			*(lpby + byMsgNum ++) = (u8)(iYcVal >> 8);
		}
		else if( (lpIEC101->dwReadAd.Word[0] >= IEC101_DDSA_2002) && (lpIEC101->dwReadAd.Word[0] <= IEC101_DDEA_2002) )//fulianqiang 2005.9.2
		{
			*(lpby + 0) = 15;
			wDdTNo = lpIEC101->dwReadAd.Word[0] - IEC101_DDSA_2002;
			//dwDdVal = GetDd(wDdTNo);
                        dwDdVal = 0x12345678;
			*(lpby + byMsgNum ++) = (u8)dwDdVal;
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 8);
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 16);
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 24);
			*(lpby + byMsgNum ++) = 0;
		}
		else
		{
			byMsgNum = 0;
		}
	}
	else
	{
#if 0          
		if ( (lpIEC101->dwReadAd.Word[0] >= IEC101_YXSA) && (lpIEC101->dwReadAd.Word[0] <= IEC101_YXEA) )//fulianqiang 2005.9.2
		{
			*(lpby + 0) = 1;
			wYxTNo = lpIEC101->dwReadAd.Word[0] - 1;
			byYxVal = GetYx(wYxTNo,YX_BIT);
			if (byYxVal)
				*(lpby + byMsgNum ++) = 1;
			else
				*(lpby + byMsgNum ++) = 0;
		}
		else
#endif                 
                if ( (lpIEC101->dwReadAd.Word[0] >= IEC101_YCSA) && (lpIEC101->dwReadAd.Word[0] <= IEC101_YCEA) )//fulianqiang 2005.9.2
		{
			*(lpby + 0) = 21;
			wYcTNo = lpIEC101->dwReadAd.Word[0] - IEC101_YCSA;
			iYcVal = GetYc(wYcTNo);
			*(lpby + byMsgNum ++) = (u8)iYcVal;
			*(lpby + byMsgNum ++) = (u8)(iYcVal >> 8);
		}
		else if( (lpIEC101->dwReadAd.Word[0] >= IEC101_DDSA) && (lpIEC101->dwReadAd.Word[0] <= IEC101_DDEA) )//fulianqiang 2005.9.2
		{
			*(lpby + 0) = 15;
			wDdTNo = lpIEC101->dwReadAd.Word[0] - IEC101_DDSA;	   //fulianqiang 2005.9.2
			dwDdVal = 0x12345678;//GetDd(wDdTNo);
			*(lpby + byMsgNum ++) = (u8)dwDdVal;
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 8);
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 16);
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 24);
			*(lpby + byMsgNum ++) = 0;
		}
		else
		{
			byMsgNum = 0;
		}
	}
#endif        
	return byMsgNum;
}
//����վ״̬�仯��Ϣ��
u8 OrgnizeSubChgMsg(void)
{
	return 0;
}

//����һ������
void SendData1(void)
{
	u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
	u8 byMsgNum = 0;
        
	switch (lpIEC101->firstData)
	{
		case yxbw:
			byMsgNum = OrgnizeYxbwMsg(lpby);
		   	lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
			lpIEC101->PSeAppLayer.LinkFunCode = 8;
			break;
		case substinit:
			byMsgNum = OrgnizeInitEndMsg(lpby);
			lpIEC101->initstatus = initend;
			lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
			lpIEC101->PSeAppLayer.LinkFunCode = 3;
			break;
	       	case readcmd:
	     		byMsgNum = OrgnizeReadDataMsg(lpby);
	       		lpIEC101->dwReadAd.Dword = 0;	     
	       		lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
	      		lpIEC101->PSeAppLayer.LinkFunCode = 8;
                        break;
		case substchange:
			byMsgNum = OrgnizeSubChgMsg();
			lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
			lpIEC101->PSeAppLayer.LinkFunCode = 8;
			break;
		case nofirstdata:
			byMsgNum = 0;
			lpIEC101->PSeAppLayer.byMsgNum = 0;
			lpIEC101->PSeAppLayer.LinkFunCode = 9;
			break;
	}
  	lpIEC101->Pacd = 0;
 	lpIEC101->firstData = nofirstdata;        
}
//��SOE��Ϣ��
u8 OrgnizeSoeMsg(void)
{
	int wSendNo;
	int  dwInfoAd;
	int wMs;
//	u8 *lpby = lpIEC101->PSeAppLayer.lpByBuf;
	u8 i,byMsgNum = 0,bySendSoeNum = 0;
	u8 * lpInfoNum;
#if 0        
	if( lpIEC101->PC56Time2a )
		*(lpby + byMsgNum ++) = 30;
	else
		*(lpby + byMsgNum ++) = 2;	 //���ͱ�ʶ	
	lpInfoNum = lpby + byMsgNum ++;
	*(lpby + byMsgNum ++) = 3;				
	if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
           *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;

       	*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
        if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
	
	//��SOEʵʱ��ȡSOE������֡������Ϣ�壩
	while (lpChanl[ChanlNo].ReadSoeC!=sHostSoe.bySoeWrite)
	{
		wSendNo = sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].byYXSendSN;
		if(wSendNo <= lpIEC101->SendYxN)
		{
			dwInfoAd.Dword = IEC101_YXSA + wSendNo ;
			dwInfoAd.Word[1] = lpIEC101->dwInfAdd.Word[1];
	        	for(i=0;i<lpIEC101->TypeInfAdd;i++)
	           		*(lpby + byMsgNum ++) = dwInfoAd.Byte[i];
			if (sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].byOnOff == 0x10)
				*(lpby + byMsgNum ++) = 0;	
			else
				*(lpby + byMsgNum ++) = 1;	
			wMs =(sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].Clock.bySecond)*1000
				+sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].Clock.wMS;
			*(lpby + byMsgNum ++) = (u8)wMs;
			*(lpby + byMsgNum ++) = (u8)(wMs >> 8);
			*(lpby + byMsgNum ++) = sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].Clock.byMinute;
			if(lpIEC101->PC56Time2a)
			{
		        	*(lpby + byMsgNum ++) = sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].Clock.byHour;
		 		*(lpby + byMsgNum ++) = sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].Clock.byDay;
		 		*(lpby + byMsgNum ++) = sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].Clock.byMonth;
		 		*(lpby + byMsgNum ++) = ( sHostSoe.SoeQueue[lpChanl[ChanlNo].ReadSoeC].Clock.wYear)-2000;
				if (++bySendSoeNum >= IEC101_SOENPF_TIME56)
					break;

		 	}
		 	else
		 	{
				if (++bySendSoeNum >= IEC101_SOENPF_TIME24)
					break;
		 	}
		}
		//�ƶ�soe��ָ��  
		if (++lpChanl[ChanlNo].ReadSoeC >= MAX_SOE_NUM)
			lpChanl[ChanlNo].ReadSoeC = 0;
	}
	if (bySendSoeNum)		
		*lpInfoNum = bySendSoeNum;	
	else
		byMsgNum = 0;			// ��SOE���������Ϊ������
#endif        
	return byMsgNum;
}
//��ң��Խ��ֵ��Ϣ��
u8 OrgnizeYcOverMsg(void)
{
	int i;
	int  dwYcAdd;
	int  nTempYc;
	int  iThreshold;	//����ֵ
	int  iDifference;	//��ֵ
	u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
	u8  j,byMsgNum = 0,bySendYcOverNum = 0;
	u8* lpInfoNum;
#if 0        
	*(lpby + byMsgNum ++) = 21;			// ���ͱ�ʶ21
	lpInfoNum = lpby + byMsgNum ++;		//��Ϣ����Ŀ
	*(lpby + byMsgNum ++) = SPONT;				// ����ԭ��<3>:ͻ��
	if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
           *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;
       	*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
        if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
	if(lpIEC101->TypeProtocol)	 //�ж��Ƿ�2002���101��Լ
 	     dwYcAdd.Dword =  IEC101_YCSA_2002;
 	else 
 	     dwYcAdd.Dword =  IEC101_YCSA;
	dwYcAdd.Word[1] = lpIEC101->dwInfAdd.Word[1];
	for (i=lpIEC101->nStartYcOver;i < lpIEC101->SendYcN;i++)
	{
		nTempYc = GetYc(i);
		iDifference=nTempYc-lpIEC101->nLastYcVal[i];	//ȡ����ң��Ĳ�ֵ
		if (iDifference<0)	//��ֵΪ����
			iDifference=-iDifference;	//����������Ϊ������
		iThreshold=(INT)((LONG)lpIEC101->nLastYcVal[i]*(LONG)lpIEC101->byRange/1000);	//��������ֵ				
		if (iThreshold<0)	//����ֵΪ����
			iThreshold=-iThreshold;	//����������Ϊ������
		if (iDifference>iThreshold)	//��ľ���ֵ�����趨������ֵ
		{
			lpIEC101->nLastYcVal[i] = nTempYc;
			dwYcAdd.Dword += i;
	        	for(j=0;j<lpIEC101->TypeInfAdd;j++)		//��Ϣ���ַ fulianqiang 2005.9.2
	          	    *(lpby + byMsgNum ++) = dwYcAdd.Byte[j];		    
			dwYcAdd.Dword -= i;
			*(lpby + byMsgNum ++) = (u8)nTempYc;	// ң��ֵ
			*(lpby + byMsgNum ++) = (u8)(nTempYc >> 8);
			if(++bySendYcOverNum >= IEC101_YCCNPF)
				break;
		}
	}
	if (i == lpIEC101->SendYcN)
		lpIEC101->nStartYcOver = 0;
	else
		lpIEC101->nStartYcOver = i;
	if (bySendYcOverNum)
		*lpInfoNum = bySendYcOverNum;
	else
		byMsgNum = 0;
#endif        
	return byMsgNum;
}
//���Ͷ�������
void SendData2(void)
{
	u8 byMsgNum = 0;
#if 1        
       	SearchFirstData();
	if (lpIEC101->firstData != nofirstdata)
	{
		lpIEC101->PSeAppLayer.LinkFunCode = 9;
		lpIEC101->PSeAppLayer.byMsgNum = 0;
		return;
	}
#if 0       
	//�¼�˳���¼����
	if (lpChanl[ChanlNo].ReadSoeC!=sHostSoe.bySoeWrite)
	{
		byMsgNum = OrgnizeSoeMsg();
		lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
		lpIEC101->PSeAppLayer.LinkFunCode = 8;
	}
#endif        
   	if (!byMsgNum)					// ��������ֵ��ң��
	{
		byMsgNum = OrgnizeYcOverMsg();
		lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
		lpIEC101->PSeAppLayer.LinkFunCode = 8;
	}      	    
	if (!byMsgNum)
	{
		lpIEC101->PSeAppLayer.byMsgNum = 0;
		//lpIEC101->PSeAppLayer.LinkFunCode = 0xE5;
                lpIEC101->PSeAppLayer.LinkFunCode = YES_ACK;
	} 
#endif        
}
//�����ٻ�ȷ����Ϣ��
u8 OrgnizeGenAck(u8 bySendReason)
{
	u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
	u8 i,byMsgNum = 0;        
	*(lpby + byMsgNum ++) = GeneralCall;		// 100
	*(lpby + byMsgNum ++) = 0x01;
	*(lpby + byMsgNum ++) = bySendReason;
	if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
           *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;

       //	*(lpby + byMsgNum ++) = lpIEC101->byCmmAdd;
       	*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
        if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
        for(i=0;i<lpIEC101->TypeInfAdd;i++)
	   *(lpby + byMsgNum ++) = 0;
	*(lpby + byMsgNum ++) = lpIEC101->byQOI;       
	return byMsgNum;
}
//��ȫң����Ϣ��
u8 OrgnizeYxMsg(u8 *lpby,u8 bySendReason,u8 byFrameNo)
{
	u8 j,byYxVal;
	u8 byMsgNum = 0;
#if 0        
    //WORD wStartAd;
 FOUR_BYTE_TO_DWORD  dwStartAd;
    int nYxNo;
	int i;
	u8  byYxNPF;	//ÿ֡ң����
    static WORD wStartYxAd[16] = {0x001, 0x041, 0x081, 0x0C1, 0x101, 0x141, 0x181, 0x1C1, 
						   0x201, 0x241, 0x281, 0x2C1, 0x301, 0x341, 0x381, 0x3C1};
	byYxNPF=lpIEC101->YxNPF[byFrameNo];	//ȡ��ʵ�ʵ�ÿ֡ң����,���һ֡���ܲ���64
	if (byFrameNo<lpIEC101->YxFN)	//֡��С����Ҫ���͵�ң��֡��
	{
		*(lpby + byMsgNum ++) = 1;		// 1���ͱ�ʶ
		*(lpby + byMsgNum ++) = 0x80 | byYxNPF;	  //�ɱ�ṹ�޶���
		*(lpby + byMsgNum ++) = bySendReason;
		if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
        	   *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;

		//*(lpby + byMsgNum ++) = lpIEC101->byCmmAdd;
       		*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
       		if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           	    *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;

		dwStartAd.Word[0] = wStartYxAd[byFrameNo];
		dwStartAd.Word[1] = lpIEC101->dwInfAdd.Word[1];
		for(j=0;j<lpIEC101->TypeInfAdd;j++)	  //��Ϣ���ַ fulianqiang 2005.9.2
		   *(lpby + byMsgNum ++) = dwStartAd.Byte[j];
		nYxNo = byFrameNo * IEC101_YXNPF;
		for (i = nYxNo; i < nYxNo + byYxNPF; i++)
		{
			byYxVal = GetYx(i,YX_BIT);
			if (byYxVal)
				*(lpby + byMsgNum ++) = 1;		// ��Ʒ�������ĵ�����Ϣ 6.4.1
			else
				*(lpby + byMsgNum ++) = 0;
		}
	}
#endif        
	return byMsgNum;
}
//��ȫң����Ϣ��
u8 OrgnizeYcMsg(u8* lpby,u8 bySendReason,u8 byFrameNo)
{
	u8 byMsgNum = 0;
	int i;
	int wYcNo;
       //	WORD wStartAd;      
        union IEC101_DADD dwStartAd;
        unsigned char databuf[8];
	unsigned long  nVal;
        float f_val;
	u8 j,byYcNPF;	//ÿ֡ң����
	static unsigned short wStartYcAd[8] = {0x701, 0x741, 0x781, 0x7C1, 0x801, 0x841, 0x881, 0x8C1};
	static unsigned short wStartYcAd2002[8] = {0x4001, 0x4020, 0x403f, 0x404e, 0x406c, 0x407b, 0x4099, 0x40a8};
	byYcNPF=lpIEC101->YcNPF[byFrameNo];	//ȡ��ʵ�ʵ�ÿ֡ң����,���һ֡���ܲ���64
	if (byFrameNo<lpIEC101->YcFN)	//֡��С����Ҫ���͵�ң��֡��
	{
		*(lpby + byMsgNum ++) = 0x0d;		// 21
		*(lpby + byMsgNum ++) = 0x80 | byYcNPF;
		*(lpby + byMsgNum ++) = bySendReason;
         	if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
                   *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;
		*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
       		if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
           	    *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;

		if (lpIEC101->TypeProtocol)	//2002��101��Լ
			dwStartAd.Word[0] = wStartYcAd2002[byFrameNo];
		else
			dwStartAd.Word[0] = wStartYcAd[byFrameNo];
		dwStartAd.Word[1] = lpIEC101->dwInfAdd.Word[1];
	        for(j=0;j<lpIEC101->TypeInfAdd;j++)
	            *(lpby + byMsgNum ++) = dwStartAd.Byte[j];
		wYcNo = byFrameNo * IEC101_YCNPF;
		for (i = wYcNo; i < wYcNo + byYcNPF; i++)
		{
			//Ϊÿһ·ң�⸳ֵ
			f_val = GetYc(i);
                        switch(i/31)
                        {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 7:
                        case 8:
                        case 9:
                          f_val = f_val/1000;
                          break;
                        case 4:
                        case 5:
                        case 6:
                          f_val = f_val/10;
                          break;
                        default:
                          break;
                        }
                        memcpy(databuf,&f_val,4);
			*(lpby + byMsgNum ++) = databuf[0];//(u8)nVal;	// ң��ֵ
			*(lpby + byMsgNum ++) = databuf[1];//(u8)(nVal >> 8);
                        *(lpby + byMsgNum ++) = databuf[2];//(u8)(nVal >> 16);
                        *(lpby + byMsgNum ++) = databuf[3];//(u8)(nVal >> 24);
                        *(lpby + byMsgNum ++) = 0;
		}
	}        
	return byMsgNum;
}
//�������ٻ�����
void SendGeneralData(void)
{ 
  u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 byMsgNum = 0;
  u8 bySendReason;
  u8 byFrameNo;
  if (lpIEC101->byPSGenStep == 0)
  {
    if (lpIEC101->byReason == ACT)
      bySendReason = ACTCON;
    else if (lpIEC101->byReason == DEACT)
      bySendReason = DEACTCON;
    byMsgNum = OrgnizeGenAck(bySendReason);
    lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
    lpIEC101->PSeAppLayer.LinkFunCode = 3;//8;
    lpIEC101->byPSGenStep++;
  }
  else if ( (lpIEC101->byPSGenStep >= 1) && (lpIEC101->byPSGenStep <= 8) )
  {
    bySendReason = lpIEC101->byQOI;
    byFrameNo = (lpIEC101->byPSGenStep - 1) * 2;
    if (lpIEC101->frameno)
    {
      byFrameNo ++;
      lpIEC101->byPSGenStep++;
    }
    lpIEC101->frameno = lpIEC101->frameno ? 0 : 1;
    byMsgNum = OrgnizeYxMsg(lpby,bySendReason,byFrameNo);
    lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
    lpIEC101->PSeAppLayer.LinkFunCode = 3;
    if (!byMsgNum)	//û��ʵ����Ϣ������
    {
      if (lpIEC101->byQOI==INTROGEN)
			//���ٻ�ʱ��Ҫ���͵�ң���ѷ���,ֱ����������ң��֡,�����ڷ����ٻ�����,�������Ϳ���Ϣ��Ӧ֡
      {
        lpIEC101->byPSGenStep = 9;	//��������ң��֡
        lpIEC101->frameno = 0;		//��һ���ڵĵ�һ֡��ʼ����
				//ִ����һ���ĳ���
        bySendReason = lpIEC101->byQOI;
	byFrameNo = (lpIEC101->byPSGenStep - 9) * 2;
        if (lpIEC101->frameno)	//��֡Ϊһ��,����1
        {
          byFrameNo ++;
          lpIEC101->byPSGenStep++;
	}
	lpIEC101->frameno = lpIEC101->frameno ? 0 : 1;
	byMsgNum = OrgnizeYcMsg(lpby,bySendReason,byFrameNo);
	lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
	lpIEC101->PSeAppLayer.LinkFunCode = 3;
        if (!byMsgNum)	//û��ʵ����Ϣ������
        {
          if (lpIEC101->byQOI==INTROGEN)	
          //���ٻ�ʱ��Ҫ���͵�ң���ѷ���,ֱ����������֡,�����ڷ����ٻ�����,�������Ϳ���Ϣ��Ӧ֡
          {
            lpIEC101->byPSGenStep = 16;	//�������ٻ�����֡
            bySendReason = ACTTERM;
            byMsgNum = OrgnizeGenAck(bySendReason);
            lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
            lpIEC101->PSeAppLayer.LinkFunCode = 3;	//���ٻ�����֡������Ϊ8
          }
          else	//�����ٻ�
		lpIEC101->PSeAppLayer.LinkFunCode = 9;	//�����������ݵ�ȷ��֡
        }
      }
      else	//�����ٻ�
        lpIEC101->PSeAppLayer.LinkFunCode = 9;	//�����������ݵ�ȷ��֡
    }
  }
  else if (lpIEC101->TypeProtocol==0)//97-101
  {
    if((lpIEC101->byPSGenStep >= 9) && (lpIEC101->byPSGenStep <= 12))
    {
      bySendReason = lpIEC101->byQOI;
      if(lpIEC101->byReason==1)
        bySendReason=lpIEC101->byReason;
      byFrameNo = (lpIEC101->byPSGenStep - 9) * 2;
      if (lpIEC101->frameno)	//��֡Ϊһ��,����1
      {
        byFrameNo ++;
        lpIEC101->byPSGenStep++;
      }
      if(lpIEC101->byPSGenStep == 13)
        lpIEC101->byPSGenStep = 15;
      lpIEC101->frameno = lpIEC101->frameno ? 0 : 1;
      byMsgNum = OrgnizeYcMsg(lpby,bySendReason,byFrameNo);
      lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
      lpIEC101->PSeAppLayer.LinkFunCode = 8;
      if(lpIEC101->byReason==1)
        lpIEC101->PSeAppLayer.LinkFunCode = 4;
      if (!byMsgNum)	//û��ʵ����Ϣ������
      {
        if (lpIEC101->byQOI==INTROGEN)	
        //���ٻ�ʱ��Ҫ���͵�ң���ѷ���,ֱ����������֡,�����ڷ����ٻ�����,�������Ϳ���Ϣ��Ӧ֡
        {
          lpIEC101->byPSGenStep = 16;	//�������ٻ�����֡
          bySendReason = ACTTERM;
          byMsgNum = OrgnizeGenAck(bySendReason);
          lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
          lpIEC101->PSeAppLayer.LinkFunCode = 8;	//���ٻ�����֡������Ϊ8
        }
        else	//�����ٻ�
          lpIEC101->PSeAppLayer.LinkFunCode = 9;	//�����������ݵ�ȷ��֡
      }
    }
  }
  else if(lpIEC101->TypeProtocol!=0)//2002-101
  {
    if((lpIEC101->byPSGenStep >= 9) && (lpIEC101->byPSGenStep <= 14))
    {
      bySendReason = lpIEC101->byQOI;
      if(lpIEC101->byReason==1)
        bySendReason=lpIEC101->byReason;
      byFrameNo = (lpIEC101->byPSGenStep - 9) * 2;
      if (lpIEC101->frameno)	//��֡Ϊһ��,����1
      {
        byFrameNo ++;
        lpIEC101->byPSGenStep++;
      }
      lpIEC101->frameno = lpIEC101->frameno ? 0 : 1;
      byMsgNum = OrgnizeYcMsg(lpby,bySendReason,byFrameNo);
      lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
      lpIEC101->PSeAppLayer.LinkFunCode = 3;
      if(lpIEC101->byReason==1)
        lpIEC101->PSeAppLayer.LinkFunCode = 4;
      if (!byMsgNum)	//û��ʵ����Ϣ������
      {
        if (lpIEC101->byQOI==INTROGEN)	
          //���ٻ�ʱ��Ҫ���͵�ң���ѷ���,ֱ����������֡,�����ڷ����ٻ�����,�������Ϳ���Ϣ��Ӧ֡
        {
          lpIEC101->byPSGenStep = 16;	//�������ٻ�����֡
          bySendReason = ACTTERM;
          byMsgNum = OrgnizeGenAck(bySendReason);
          lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
          lpIEC101->PSeAppLayer.LinkFunCode = 3;	//���ٻ�����֡������Ϊ8
        }
	else	//�����ٻ�
          lpIEC101->PSeAppLayer.LinkFunCode = 9;	//�����������ݵ�ȷ��֡
      }
    }
  }
  else if (lpIEC101->byPSGenStep == 15)
  {
    bySendReason = ACTTERM;
    byMsgNum = OrgnizeGenAck(bySendReason);
    lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
    lpIEC101->PSeAppLayer.LinkFunCode = 8;	//���ٻ�����֡������Ϊ8
    lpIEC101->byPSGenStep++;
  }       
}
//��ʱ����Ϣ��
u8 OrgnizeTimeMsg(void)
{ 
	unsigned short wMS;
        unsigned char Buff[8];
        unsigned char* Point;
        Point = Buff;
        HT_RTC_Read(Point);
	u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
	u8 j,byMsgNum = 0;
	u8 byTimeValid = 0;

	*(lpby + byMsgNum ++) = 103;	// 103
	*(lpby + byMsgNum ++) = 1;
        if(lpIEC101->byReason == ACT)
          *(lpby + byMsgNum ++) = ACTCON;
        else
          *(lpby + byMsgNum ++) = REQ;
	if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
           *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;

	*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
       	if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
             *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;

	for(j=0;j<lpIEC101->TypeInfAdd;j++)
	    *(lpby + byMsgNum ++) = 0;

	//ȡϵͳʱ��
	wMS = *(Point+6) * 1000;
	*(lpby + byMsgNum ++) = (u8)wMS;					// ����
	*(lpby + byMsgNum ++) = (u8)(wMS >> 8);
	*(lpby + byMsgNum ++) = 0 << 7 | *(Point+5);	// IV = 0 ��Ч RES1 = 0
	*(lpby + byMsgNum ++) = *(Point+4);					// SU = 0 ��׼ʱ�� RES2 = 0
	*(lpby + byMsgNum ++) = *(Point+3) | (*Point) << 5;
	*(lpby + byMsgNum ++) = *(Point+2);					// RES3 = 0
	*(lpby + byMsgNum ++) = *(Point+1);
       	return byMsgNum;       
}

void SendTimeAck(void)
{
  u8 byMsgNum = 0;
  byMsgNum = OrgnizeTimeMsg();
  lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
  lpIEC101->PSeAppLayer.LinkFunCode = 8;       
}
//���ٻ����ȷ����Ϣ��
u8 OrgnizeDdAckMsg(u8 bySendReason)
{ 
	u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
	u8 j,byMsgNum = 0;
	*(lpby + byMsgNum ++) = C_CI_NA_1;		// 101
	*(lpby + byMsgNum ++) = 0x81;
	*(lpby + byMsgNum ++) = bySendReason;
	if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
           *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;
	*(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
       	if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
             *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
	for(j=0;j<lpIEC101->TypeInfAdd;j++)
	    *(lpby + byMsgNum ++) = 0;

	*(lpby + byMsgNum ++) = lpIEC101->byQCC;
	return byMsgNum;      
}
//����������Ϣ��
u8 OrgnizeDdMsg(u8 bySendReason,u8 byFrameNo)
{
  int i;
  FOUR_BYTE_TO_DWORD  dwInfoAd;
  int dwDdVal;
  u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 j,byMsgNum = 0;
  u8 byDdNPF;	//ÿ֡�����
  int dwDdAdd;
  byDdNPF=lpIEC101->DdNPF[byFrameNo];	//ȡ��ʵ�ʵ�ÿ֡�����,���һ֡���ܲ���32
  if (byFrameNo<lpIEC101->DdFN)	//֡��С����Ҫ���͵ĵ��֡��
  {
    if(byFrameNo%2)
    {
      *(lpby + byMsgNum ++) = M_IT_TC_1;	
      *(lpby + byMsgNum ++) = 0x80|byDdNPF;
     }
     else
     {
      *(lpby + byMsgNum ++) = M_IT_NB_1;	
      *(lpby + byMsgNum ++) = 0x80|byDdNPF;
     }
      *(lpby + byMsgNum ++) = bySendReason;
      if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
        *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;
      *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
      if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
      *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
      if(lpIEC101->TypeProtocol)
        dwDdAdd =  IEC101_DDSA_2002+31;
      else
        dwDdAdd =  IEC101_DDSA;
      dwInfoAd.Dword = dwDdAdd+(byFrameNo/2)*31;
      dwInfoAd.Word[1] = lpIEC101->dwInfAdd.Word[1];
      for(j=0;j<lpIEC101->TypeInfAdd;j++)
        *(lpby + byMsgNum ++) = dwInfoAd.Byte[j];
      for (i = byFrameNo * IEC101_DDNPF; i < byFrameNo * IEC101_DDNPF + byDdNPF; i++)
      {
#if 0                        
			dwDdVal = 0;			// ��ȷ���ֵ
			dwDdVal = i;//GetDd(i);
			*(lpby + byMsgNum ++) = (u8)dwDdVal;
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 8);
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 16);
			*(lpby + byMsgNum ++) = (u8)(dwDdVal >> 24);
			*(lpby + byMsgNum ++) = i - byFrameNo * IEC101_DDNPF;	// IV CA CY SQ--6.4.7
#else
                        dwDdVal=GetDd(i,lpby + byMsgNum);
                        byMsgNum +=dwDdVal;
                        if(byFrameNo%2==0)
                          *(lpby + byMsgNum ++) = 0;
                       // byMsgNum +=dwDdVal;
                       // *(lpby + byMsgNum ++) = i - byFrameNo * IEC101_DDNPF;
#if 0                       
                        if(dwDdVal==4)
                        {
                          byMsgNum +=dwDdVal;
                          *(lpby + byMsgNum ++) = i - byFrameNo * IEC101_DDNPF;
                        }
                        else
                        {
                           *(lpby + byMsgNum+4) = i - byFrameNo * IEC101_DDNPF;
                           byMsgNum +=dwDdVal;
                        }
#endif                        
                        
#endif                        
		}
	}
	return byMsgNum;
}
//Ϊ�˼���97��2002����ٻ��������ֽ��������
//1���Ƚ�ֱ�۵Ľ���취����дһ��ƺ����ֱ���ã������ô˷���
//2����һ�ֽ��������bl2002Ϊ�Ƿ�2002����б�������ڵ�Ⱥ����������´����滻ԭ���Ĵ��룬�η������Լ��ٴ�����
		//if((��bl2000==TRUE)&&(lpIEC101->byQCC&0X3F)==5))||(��bl2000==FALSE)&&(lpIEC101->byQCC&0X3F)==1)))
		 //   bySendReason = REQCGCN;
		 //else
		 //   bySendReason = (lpIEC101->byQCC & 0x3f) - 1 + REQCOGCN;// ������lpIEC101->byPSDdStep��ֵ��ʱ��������
		 //���������
		 //if(bl2002)
		 //{
		 //  if(  lpIEC101->byQCC&0X3F)==5)
		 //  lpIEC101->byPSDdStep=0;
		 //  else
		 //  lpIEC101->byPSDdStep=lpIEC101->byQCC&0X3F;
		 //}
		 //else
		 // lpIEC101->byPSDdStep=lpIEC101->byQCC&0X3F-1; 
		 //�����ĵط�����Ҫ��   
//���͵������
void SendDdData(void)
{
        
}
//2002��ĵ���ٻ�����	  //��2005.8.30
void SendDdData2002(void)
{
        u8 byMsgNum = 0;
        u8 bySendReason;
        u8 byFrameNo;
	if (lpIEC101->byPSDdStep == 17 || lpIEC101->byPSDdStep == 0)
	{
		if (lpIEC101->byReason == ACT)
			bySendReason = ACTCON;
		else if (lpIEC101->byReason == DEACT)
			bySendReason = DEACTCON;
		byMsgNum = OrgnizeDdAckMsg(bySendReason);
		lpIEC101->byPSDdStep=0;
		lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
		lpIEC101->PSeAppLayer.LinkFunCode = 3;
	}
	else if ( (lpIEC101->byPSDdStep >= 1) && (lpIEC101->byPSDdStep <= 16) )
	{
		byFrameNo = lpIEC101->byPSDdStep - 1;
		if ( (lpIEC101->byQCC & 0x3f) == 5)
			bySendReason = REQCOGCN;		// 37��Ӧ֡�Ĵ���ԭ��
		else if ( ((lpIEC101->byQCC & 0x3f) >= 1) 
			&& ((lpIEC101->byQCC & 0x3f) <= 4) )
			bySendReason = (lpIEC101->byQCC & 0x3f) - 1 + REQCOGCN;
		byMsgNum = OrgnizeDdMsg(bySendReason,byFrameNo);
		lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
		lpIEC101->PSeAppLayer.LinkFunCode = 3;
		if ( lpIEC101->byPSDdStep==lpIEC101->DdFN)
		     lpIEC101->byPSDdStep=17;
		if (!byMsgNum)	//û��ʵ�ʵ����Ϣ������
		{
			if (bySendReason==REQCOGCN)
			//��Ӧ�������ٻ�ʱ��Ҫ���͵ĵ���ѷ���,ֱ����������֡,�����ڷ����ٻ�����,�������Ϳ���Ϣ��Ӧ֡
			{
				lpIEC101->byPSDdStep = 17;	
				bySendReason = ACTTERM;
                               // lpIEC101->byReason=ACTTERM;
				byMsgNum = OrgnizeDdAckMsg(bySendReason);
				lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
				lpIEC101->PSeAppLayer.LinkFunCode = 3;
			}
			else	//�����ٻ�
				lpIEC101->PSeAppLayer.LinkFunCode = 9;	//�����������ݵ�ȷ��֡
		}	
	}       
	else if(lpIEC101->byPSDdStep == 18)
	{
               // lpIEC101->byReason=ACTTERM;
		bySendReason = ACTTERM;
		byMsgNum = OrgnizeDdAckMsg(bySendReason);
		lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
		lpIEC101->PSeAppLayer.LinkFunCode = 3;
	}     
	lpIEC101->byPSDdStep++;
        
}

//�����������ȷ��֡
void SendReadDataAck(void)
{
  lpIEC101->PSeAppLayer.byMsgNum = 0;
  lpIEC101->PSeAppLayer.LinkFunCode = 0;

}
//��ң�ط�У��Ϣ��
u8 OrgnizeYkRetMsg(u8 bySendReason)
{

	return 0;
}
//��CAN���·�ң������
void ReceiveYktoCan(void)
{
	
}
//��ң�ط�У
void SendYkReply(void)
{
	
}
//��ͨ��������Ϣ��
u8 OrgnizeTestMsg(void)
{
  u8 *lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 j,byMsgNum = 0;
  *(lpby + byMsgNum ++) = 104;			// 104
  *(lpby + byMsgNum ++) = 0;//1;
  *(lpby + byMsgNum ++) = ACTCON;		// 7
  if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
    *(lpby + byMsgNum ++) = 0;//lpIEC101->bySourceAdd;
  *(lpby + byMsgNum ++) = 0x01;//lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
  if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
    *(lpby + byMsgNum ++) = 0x00;//lpIEC101->wCmmAdd.Byte.h;
  for(j=0;j<lpIEC101->TypeInfAdd;j++)
    *(lpby + byMsgNum ++) = 0;
  *(lpby + byMsgNum ++) = lpIEC101->wTester.Byte.l;
  *(lpby + byMsgNum ++) = lpIEC101->wTester.Byte.h;
  lpIEC101->wTester.Byte.l =0;
  lpIEC101->wTester.Byte.h =0;
  return byMsgNum;
}
//����ͨ������ȷ��
void SendChanTestAck(void)
{
  u8 byMsgNum = 0;
  byMsgNum = OrgnizeTestMsg();
  lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
  lpIEC101->PSeAppLayer.LinkFunCode = 3;
  lpIEC101->Pacd = 0x2;
  lpIEC101->Pdfc = 1;
}

void Dir_Send(void)
{
  u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 i,byMsgNum = 0;        
  if(lpIEC101->byPSGenStep>1)
  {
    return;
  }
  *(lpby + byMsgNum ++) = F_FR_NA_1;
  *(lpby + byMsgNum ++) = 0x01;
  *(lpby + byMsgNum ++) = REQ;
  if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
    *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;
  *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
  if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
    *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
  for(i=0;i<lpIEC101->TypeInfAdd;i++)
    *(lpby + byMsgNum ++) = 0;
  *(lpby + byMsgNum ++) = 0x02; //
  *(lpby + byMsgNum ++) = 0x02;
  *(lpby + byMsgNum ++) = 0x00;
  memcpy(lpby + byMsgNum,&(lpIEC101->FId),4);byMsgNum+=4;
  if(lpIEC101->byPSGenStep)
  {
    *(lpby + byMsgNum ++) = 0x00;
  }
  else
  {
    *(lpby + byMsgNum ++) = 0x01;
  }
  *(lpby + byMsgNum ++) = 0x05;
  for(i=0;i<5;++i)
  {
    *(lpby + byMsgNum) = strlen(File_List[i+lpIEC101->byPSGenStep*5]);
    memcpy((lpby + byMsgNum + 1),File_List[i+lpIEC101->byPSGenStep*5],*(lpby + byMsgNum));
    byMsgNum += (*(lpby + byMsgNum)+1);
    *(lpby + byMsgNum++) = 0;
    *(lpby + byMsgNum++) = 0x88+i+lpIEC101->byPSGenStep*5;
    *(lpby + byMsgNum++) = 0;
    *(lpby + byMsgNum++) = 0;
    *(lpby + byMsgNum++) = 0;
    *(lpby + byMsgNum++) = 0x50;
    *(lpby + byMsgNum++) = 0x46; 
    *(lpby + byMsgNum++) = 0x20;
    *(lpby + byMsgNum++) = 0x0A;
    *(lpby + byMsgNum++) = 0x1B;
    *(lpby + byMsgNum++) = 0x0A;
    *(lpby + byMsgNum++) = 0x11;
  }
  lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
  lpIEC101->PSeAppLayer.LinkFunCode = 3;
  lpIEC101->byPSGenStep++;
}

void File_Recv(void)
{
}

//�ļ��ٻ�ȷ����Ϣ��
u8 SendFileGenAck(u8 bySendReason)
{
  u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 i,byMsgNum = 0;
  u32 m_filelen;
  *(lpby + byMsgNum ++) = F_FR_NA_1;		// 210
  *(lpby + byMsgNum ++) = 0x01;
  *(lpby + byMsgNum ++) = bySendReason;
  if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
    *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;;
  *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
  if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
    *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
  for(i=0;i<lpIEC101->TypeInfAdd;i++)
    *(lpby + byMsgNum ++) = 0;
  *(lpby + byMsgNum ++) = 0x2;    
  *(lpby + byMsgNum ++) = 0x4;
  *(lpby + byMsgNum ++) = 0x0;
  *(lpby + byMsgNum ++) = strlen(lpIEC101->Fname);
  strcpy(lpby + byMsgNum,lpIEC101->Fname);
  byMsgNum += strlen(lpIEC101->Fname);
  memcpy(lpby + byMsgNum,&(lpIEC101->FId),4);
  byMsgNum += 4;
  memcpy(lpby + byMsgNum,&(m_filelen),4);
  byMsgNum += 4;
  return byMsgNum;
}

//�ļ���Ϣ��
u8 SendFileInfo(u8 bySendReason)
{
  u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 i,byMsgNum = 0,pos,bySum=0,j;
  char *p_filename;
  u32 m_filelen;
  *(lpby + byMsgNum ++) = F_FR_NA_1;		// 210
  *(lpby + byMsgNum ++) = 0x01;
  *(lpby + byMsgNum ++) = 5;
  if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
    *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;;
  *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
  if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
    *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
  for(i=0;i<lpIEC101->TypeInfAdd;i++)
    *(lpby + byMsgNum ++) = 0;
  *(lpby + byMsgNum ++) = 0x2;    
  *(lpby + byMsgNum ++) = 0x5;
  memcpy(lpby + byMsgNum,&(lpIEC101->FId),4);
  byMsgNum += 4;
  m_filelen = 0;
  memcpy(lpby + byMsgNum,&(m_filelen),4);
  byMsgNum += 4;
  *(lpby + byMsgNum ++) = 0x1;
  pos = byMsgNum;
#if 0  
  strcpy(lpby + byMsgNum,lpIEC101->Fname+14);
  byMsgNum += strlen(lpIEC101->Fname+14);
#else
  p_filename = lpIEC101->Fname;
  if(p_filename)
  {
    p_filename = (char *)strstr((const char *)p_filename,"/");
    if(p_filename)
    {
      p_filename += 1;
      p_filename = (char *)strstr(p_filename,"/");
    }
    p_filename += 1;
    strcpy(lpby + byMsgNum,p_filename);
    byMsgNum += strlen(p_filename);
  }
#endif  
  strcpy(lpby + byMsgNum,"  v1.0\r\n");
  byMsgNum += strlen("  v1.0\r\n");
  strcpy(lpby + byMsgNum,"201710300001,96,2\r\n");
  byMsgNum += strlen("201710300001,96,2\r\n");
  for(j=pos;j<byMsgNum;j++)
    bySum = bySum + *(lpby + j);
  *(lpby + byMsgNum ++) = bySum;
  return byMsgNum;
}


//�ļ�����
u8 SendFileData(u8 bySendReason)
{
  u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 i,byMsgNum = 0,pos,bySum=0,j;
  u32 m_filelen;
  *(lpby + byMsgNum ++) = F_FR_NA_1;		// 210
  *(lpby + byMsgNum ++) = 0x01;
  *(lpby + byMsgNum ++) = 5;
  if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
    *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;;
  *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
  if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
    *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
  for(i=0;i<lpIEC101->TypeInfAdd;i++)
    *(lpby + byMsgNum ++) = 0;
  *(lpby + byMsgNum ++) = 0x2;    
  *(lpby + byMsgNum ++) = 0x5;
  memcpy(lpby + byMsgNum,&(lpIEC101->FId),4);
  byMsgNum += 4;
  m_filelen = (lpIEC101->byPSGenStep-1)*256;
  memcpy(lpby + byMsgNum,&(m_filelen),4);
  byMsgNum += 4;
  if(lpIEC101->byPSGenStep<9)
    *(lpby + byMsgNum ++) = 0x1;
  else
    *(lpby + byMsgNum ++) = 0x0;
  pos = byMsgNum;
  if(strstr(lpIEC101->Fname,".msg"))
  {
    strcpy(lpby + byMsgNum,"12,2017-10-24 15:15:00,6401,   10.71,6402,    0.23,6403,    0.23,6404,    0.08,6405,    0.00,6406,    0.08,6407,    0.00,6408,    0.00,4008,-    0.003,4009,-    0.003,400A,-    0.003,400B,-    0.006\r\n");
    byMsgNum += strlen("12,2017-10-24 15:15:00,6401,   10.71,6402,    0.23,6403,    0.23,6404,    0.08,6405,    0.00,6406,    0.08,6407,    0.00,6408,    0.00,4008,-    0.003,4009,-    0.003,400A,-    0.003,400B,-    0.006\r\n");
  }
  else if(strstr(lpIEC101->Fname,".xml"))
  {
    switch(lpIEC101->byPSGenStep)
    {
    case 2:
      strcpy(lpby + byMsgNum,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<DataFile>\r\n\t<Header fileType=\"FIXD\" fileVer=\"1.00\" devName=\"201709030019\" />\r\n\t<DataAttr dataNum=\"12\" sectNum=\"96\" interval=\"15min\">\r\n");
      byMsgNum += strlen("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<DataFile>\r\n\t<Header fileType=\"FIXD\" fileVer=\"1.00\" devName=\"201709030019\" />\r\n\t<DataAttr dataNum=\"12\" sectNum=\"96\" interval=\"15min\">\r\n");
      break;
    case 3:
      strcpy(lpby + byMsgNum,"\t\t<DI ioa=\"25601\" type=\"float\" unit=\"kWh\" />\r\n \
    \t\t<DI ioa=\"25602\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25603\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25604\" type=\"float\" unit=\"kVARh\" />\r\n");
      byMsgNum += strlen("\t\t<DI ioa=\"25601\" type=\"float\" unit=\"kWh\" />\r\n \
    \t\t<DI ioa=\"25602\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25603\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25604\" type=\"float\" unit=\"kVARh\" />\r\n");
      break;
    case 4:
      strcpy(lpby + byMsgNum,"\t\t<DI ioa=\"25601\" type=\"float\" unit=\"kWh\" />\r\n \
    \t\t<DI ioa=\"25605\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25606\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25607\" type=\"float\" unit=\"kVARh\" />\r\n");
      byMsgNum += strlen("\t\t<DI ioa=\"25601\" type=\"float\" unit=\"kWh\" />\r\n \
    \t\t<DI ioa=\"25605\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25606\" type=\"float\" unit=\"kVARh\" />\r\n \
    \t\t<DI ioa=\"25607\" type=\"float\" unit=\"kVARh\" />\r\n");
      break;
    case 5:
      strcpy(lpby + byMsgNum,"\t\t<DI ioa=\"16392\" type=\"float\" unit=\"W\" />\r\n \
    \t\t<DI ioa=\"16393\" type=\"float\" unit=\"W\" />\r\n \
    \t\t<DI ioa=\"16394\" type=\"float\" unit=\"W\" />\r\n \
    \t\t<DI ioa=\"16395\" type=\"float\" unit=\"W\" />\r\n \
  \t</DataAttr>\r\n");
      byMsgNum += strlen("\t\t<DI ioa=\"16392\" type=\"float\" unit=\"W\" />\r\n \
    \t\t<DI ioa=\"16393\" type=\"float\" unit=\"W\" />\r\n \
    \t\t<DI ioa=\"16394\" type=\"float\" unit=\"W\" />\r\n \
    \t\t<DI ioa=\"16395\" type=\"float\" unit=\"W\" />\r\n \
  \t</DataAttr>\r\n");
             break;
    default:
      if(lpIEC101->byPSGenStep%2==0)
      {
      strcpy(lpby + byMsgNum,"\t<DataRec sect=\"1\" tm=\"20171025_113000\">\r\n \
      \t\t<DI val=\"10.718\"/>\r\n \
      \t\t<DI val=\"0.236\"/>\r\n \
      \t\t<DI val=\"0.236\"/>\r\n \
      \t\t<DI val=\"0.082\"/>\r\n \
      \t\t<DI val=\"0.0\"/>\r\n \
      \t\t<DI val=\"0.082\"/>\r\n");
      byMsgNum += strlen("\t<DataRec sect=\"1\" tm=\"20171025_113000\">\r\n \
      \t\t<DI val=\"10.718\"/>\r\n \
      \t\t<DI val=\"0.236\"/>\r\n \
      \t\t<DI val=\"0.236\"/>\r\n \
      \t\t<DI val=\"0.082\"/>\r\n \
      \t\t<DI val=\"0.0\"/>\r\n \
      \t\t<DI val=\"0.082\"/>\r\n");
      }
      else
      {
        strcpy(lpby + byMsgNum,"\t\t<DI val=\"10.718\"/>\r\n \
        \t<DI val=\"0.236\"/>\r\n \
        \t<DI val=\"0.236\"/>\r\n \
        \t<DI val=\"0.082\"/>\r\n \
        \t<DI val=\"0.0\"/>\r\n \
        \t<DI val=\"0.082\"/>\r\n\t</DataRec>\r\n");
        byMsgNum += strlen("\t\t<DI val=\"10.718\"/>\r\n \
        \t<DI val=\"0.236\"/>\r\n \
        \t<DI val=\"0.236\"/>\r\n \
        \t<DI val=\"0.082\"/>\r\n \
        \t<DI val=\"0.0\"/>\r\n \
        \t<DI val=\"0.082\"/>\r\n\t</DataRec>\r\n");
        if(lpIEC101->byPSGenStep>=9)
        {
          strcpy(lpby + byMsgNum,"</DataFile>\r\n");
          byMsgNum += strlen("</DataFile>\r\n");
        }
      } 
      break;
    }
  }
   for(j=pos;j<byMsgNum;j++)
    bySum = bySum + *(lpby + j);
  *(lpby + byMsgNum ++) = bySum;
  return byMsgNum;
}

void File_Send(void)
{
  u8 bySendReason;
  u8 byMsgNum=0;
  if(lpIEC101->byPSGenStep)
  {
    if(lpIEC101->byPSGenStep==1)
    {
      byMsgNum=SendFileInfo(1);
      lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
      lpIEC101->PSeAppLayer.LinkFunCode = 3;
    }
    else if(lpIEC101->byPSGenStep<10)
    {
      byMsgNum=SendFileData(1);
      lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
      lpIEC101->PSeAppLayer.LinkFunCode = 3;
    }
  }
  else
  {
    if (lpIEC101->byReason == ACT)
      bySendReason = ACTCON;
    else if (lpIEC101->byReason == DEACT)
      bySendReason = DEACTCON;
    byMsgNum = SendFileGenAck(bySendReason);
    lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
    lpIEC101->PSeAppLayer.LinkFunCode = 3;
  }
  ++lpIEC101->byPSGenStep;
}

void File_Server(void)
{
  switch(lpIEC101->Fop)
  {
  case F_FR_DA_1:
    Dir_Send();
    break;
  case F_FR_FA_1:
    File_Send();
    break;
  case F_FW_FA_1:
      File_Recv();
    break;
  default:
    break;
  }
}
void Sn_Read(void)
{
  u8* lpby = lpIEC101->PSeAppLayer.lpByBuf;
  u8 i,byMsgNum = 0,j;        
  *(lpby + byMsgNum ++) = C_RS_NA_1;
  *(lpby + byMsgNum ++) = lpIEC101->byQualify;
  *(lpby + byMsgNum ++) = ACTCON;
  if (lpIEC101->TypeSeReason==2)			//����ԭ�������ֽ�
    *(lpby + byMsgNum ++) = lpIEC101->bySourceAdd;
  *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.l;   //2005.9.2
  if(lpIEC101->TypeCmmAdd==2)	   //�Ƿ������ֽڵ�Ӧ�÷���Ԫ��ַ
    *(lpby + byMsgNum ++) = lpIEC101->wCmmAdd.Byte.h;
  *(lpby + byMsgNum ++) = lpIEC101->Sn&0xff; 
  *(lpby + byMsgNum ++) = (lpIEC101->Sn>>8)&0xff; 
  *(lpby + byMsgNum ++) = 0x00;
 
  for(i=0;i<lpIEC101->byQualify;++i)
  {
    *(lpby + byMsgNum ++) = lpIEC101->List_para[i]&0xff; 
    *(lpby + byMsgNum ++) = (lpIEC101->List_para[i]>>8)&0xff; 
    *(lpby + byMsgNum ++) = 4;
    for(j=0;j<9;++j)
    {
      if(lpIEC101->List_para[i]==s_sninfo[j].info_addr)
        break;
    }
    if(j<9)
    {
      *(lpby + byMsgNum ++) = s_sninfo[j].data_len;
      memcpy(lpby + byMsgNum,s_sninfo[j].info_data,s_sninfo[j].data_len);
      byMsgNum += s_sninfo[j].data_len;
    }
    else
    {
      *(lpby + byMsgNum ++) = 0;
    }
  }
  lpIEC101->PSeAppLayer.byMsgNum = byMsgNum;
  lpIEC101->PSeAppLayer.LinkFunCode = 3;
  lpIEC101->byPSGenStep++;
  lpIEC101->PReMsgType = 0;
}
//����Ϣ֡
void AppVFrame(void)
{
  switch (lpIEC101->PReMsgType)
  {
  case GeneralCall:
      SendGeneralData();
      lpIEC101->PSeAppLayer.byFull = 1;
      if ( (lpIEC101->byReason == ACT) && (lpIEC101->byPSGenStep == 16) )
        lpIEC101->PReMsgType = 0;
      else if( (lpIEC101->byReason == REQ) && (!lpIEC101->frameno) )
        lpIEC101->PReMsgType = 0;
      else if(lpIEC101->byReason == DEACT)
        lpIEC101->PReMsgType = 0;
      break;
    case C_CS_NA_1:				//Уʱȷ��
      SendTimeAck();
      lpIEC101->PSeAppLayer.byFull = 1;
      lpIEC101->PReMsgType = 0;
      break;
    case C_CI_NA_1:				//�ٻ����
      if ( lpIEC101->TypeProtocol)//2002���101��Լ//��2005.8.30
        SendDdData2002();
      else
        SendDdData();
      lpIEC101->PSeAppLayer.byFull = 1;
    
      if( (lpIEC101->byReason == ACT) && (lpIEC101->byPSDdStep == 17) )
        lpIEC101->PReMsgType = 0;
      else if(lpIEC101->byReason == REQ)
        lpIEC101->PReMsgType = 0;
    else if((lpIEC101->byReason == DEACT))
      lpIEC101->PReMsgType = 0;
    else if(lpIEC101->byPSDdStep > 18)
      lpIEC101->PReMsgType = 0;
    break;
   case C_RD_NA_1:				//������ȷ��
     SendReadDataAck();
     lpIEC101->PSeAppLayer.byFull = 1;
     lpIEC101->PReMsgType = 0;
     break;
    case C_SC_NA_1:				//����ң��
    case C_DC_NA_1:				//ң��
    case C_RC_NA_1:				//��������
      SendYkReply();
      if( (lpIEC101->WaitYkXzRet == 0) && (lpIEC101->WaitYkZxRet == 0) )
      {
        lpIEC101->PSeAppLayer.byFull = 1;
        lpIEC101->PReMsgType = 0;
      }
      if (lpIEC101->SendYkZxAck == 1)
      {
        lpIEC101->PSeAppLayer.byFull = 1;
        lpIEC101->SendYkZxAck = 0;
      }
      break;
  case  C_TS_NA_1:
    SendChanTestAck();
    lpIEC101->PReMsgType = 0;
    lpIEC101->PWinTimer=0;
    lpIEC101->PSeAppLayer.byFull = 1;
    break;
  case C_RS_NA_1:
    Sn_Read();
    if(lpIEC101->PSeAppLayer.byMsgNum)
      lpIEC101->PSeAppLayer.byFull = 1;
    else
    {
      lpIEC101->PReMsgType = 0;
    }
    break;
  case F_FR_NA_1: //�ļ�����
    File_Server();
    if(lpIEC101->PSeAppLayer.byMsgNum)
      lpIEC101->PSeAppLayer.byFull = 1;
    else
    {
      lpIEC101->PReMsgType = 0;
    }
    break;
  default:
    lpIEC101->PSeAppLayer.byFull = 0;
    lpIEC101->PReMsgType = 0;
    break;
  }
}
//�Ӷ�Ӧ�ò㷢�ʹ�������
void PAppSendProcess(void)
{
  if (lpIEC101->PReFrameType == CALL_DATA1)
  {
    SendData1();
    lpIEC101->PSeAppLayer.byFull = 1;
    lpIEC101->PReFrameType = 0;
  }
  else  
  {
    if(3==lpIEC101->UnsolTimeInterval)
    {
     // if(lpIEC101->PReFrameType)// == CALL_DATA2)
      {
        if( 0==lpIEC101->PReFrameType)
        {
          SendData2();
          lpIEC101->PSeAppLayer.byFull = 1;
          lpIEC101->PReFrameType = 0xff;
        }
        else
        {
          //if (((lpIEC101->PWinTimer >= 200)&& (lpIEC101->PReFrameType==0x3)))
          if((lpIEC101->OrgnizeFrame) && (lpIEC101->PWinTimer >= 100))
          { 
           // lpIEC101->PWinTimer =0 ;
            AppVFrame();
            lpIEC101->PReFrameType = 0xff;
          }
        }
      }
    }
    else
    {
      //if(lpIEC101->PReFrameType == CALL_DATA2)
      //if((lpIEC101->PReFrameType == YES_ACK) || (lpIEC101->PReFrameType == CALL_DATA2))
      if(lpIEC101->PReFrameType && lpIEC101->PReFrameType!=0xff)
      {
#if 0        
        if(lpIEC101->Pacd==0x2)
        {
          lpIEC101->PSeAppLayer.byFull=1;
         // AppVFrame();
          lpIEC101->PReFrameType = 0xff;
        }
        else
#endif          
        {
          SendData2();
          lpIEC101->PSeAppLayer.byFull = 1;
          lpIEC101->PReFrameType = 0xff;
          lpIEC101->Pacd=0x2;
        }
      }
      else if(lpIEC101->Pacd==0x2)
      {
        AppVFrame();
      }
 /*     
      else
      {
        if (lpIEC101->PWinTimer >= 5000)
        {
          
          AppVFrame();
          if(lpIEC101->PSeAppLayer.byFull)
            lpIEC101->PWinTimer=0;
        }
      }
      */
    }
  }

}
//Ӧ�ò㷢�ʹ�������
void Iec101AppSendPro(void)
{
	if ( (lpIEC101->PReFrameType!=0xff || lpIEC101->PReMsgType)
		&& (lpIEC101->PSendFrame.byFull == 0)&&(lpIEC101->PSeAppLayer.byFull == 0))
	{
		PAppSendProcess();
	}
}
//��䳤��·֡
u8 OrgnizeVarFrame(u8 *lpby,u8 byMsgNum,u8 byConField)	//fulianqiang 2005.9.2
{
	int i;
	u8 bySum = 0;
	//u8 byMsgNum = lpIEC101->PSeAppLayer.byMsgNum;
	*(lpby + 0) = V_STARTCODE;	// �䳤֡��ʼ�ֽ�
	*(lpby + 1) = byMsgNum + 1 + lpIEC101->TypeLinkAdd;
	*(lpby + 2) = byMsgNum + 1 + lpIEC101->TypeLinkAdd;
	*(lpby + 3) = V_STARTCODE;	// �䳤֡��ʼ�ֽ�
	*(lpby + 4) = byConField;
	*(lpby + 5) = lpIEC101->wLinkAdd.Byte.l;
	if( lpIEC101->TypeLinkAdd ==2)
	   *(lpby + 6) = lpIEC101->wLinkAdd.Byte.h;
	for (i = 4; i < byMsgNum+5+lpIEC101->TypeLinkAdd ; i++)
		bySum = bySum + *(lpby + i);
	*(lpby + byMsgNum + 5 + lpIEC101->TypeLinkAdd) = bySum;
	*(lpby + byMsgNum + 6 + lpIEC101->TypeLinkAdd) = STOPCODE;	// ֹͣ�ֽ�
	return byMsgNum + 7 + lpIEC101->TypeLinkAdd;
}
//�鶨����·֡
u8 OrgnizeFixFrame(u8 *lpby,u8 byConField)
{
  u8 j,bySum = 0,i=0;
  *(lpby + i++) = F_STARTCODE;	// ����֡��ʼ�ֽ�
  *(lpby + i++) = byConField;
  *(lpby + i++) = lpIEC101->wLinkAdd.Byte.l;
  if( lpIEC101->TypeLinkAdd ==2)
    *(lpby + i++) = lpIEC101->wLinkAdd.Byte.h;
  for(j=1;j<i;j++)
    bySum = bySum + *(lpby + j);
  *(lpby + i++) = bySum;
  *(lpby + i++) = STOPCODE;	// ֹͣ�ֽ�
  return i;
}
//�Ӷ���·�㷢�ʹ�������
void PLinkSendProcess(void)
{
	u8 byConField;
	lpIEC101->PSendFrame.byFunCode = lpIEC101->PSeAppLayer.LinkFunCode;
        
       if(lpIEC101->FlagPingH)       
         byConField = 0x80 | (lpIEC101->Pacd << 5) | (lpIEC101->Pdfc << 4)
					| lpIEC101->PSendFrame.byFunCode;
       else
          byConField = (lpIEC101->Pacd << 5) | (lpIEC101->Pdfc << 4)
					| lpIEC101->PSendFrame.byFunCode;
	if (lpIEC101->PSeAppLayer.byMsgNum)
	{
		lpIEC101->PSendFrame.wFrameLen = 
                OrgnizeVarFrame(lpIEC101->PSendFrame.byLinkBuf,lpIEC101->PSeAppLayer.byMsgNum,byConField);
                lpIEC101->PSeAppLayer.byMsgNum=0;  //zzl add
	}
	else
	{
		if (lpIEC101->PSendFrame.byFunCode == 0xE5)
		{
			lpIEC101->PSendFrame.byLinkBuf[0] = 0xE5;
			lpIEC101->PSendFrame.wFrameLen = 1;
		}
		else
		{
			lpIEC101->PSendFrame.wFrameLen = 
                        OrgnizeFixFrame(lpIEC101->PSendFrame.byLinkBuf,byConField);
		}
	}
	lpIEC101->PSendFrame.byFull = 1;
}
//��·�㷢�ʹ�������
void Iec101LinkSendPro(void)
{
	if (lpIEC101->PSeAppLayer.byFull == 1)
	{
		PLinkSendProcess();
		lpIEC101->PSeAppLayer.byFull = 0;
	}
}
//��·�㷢�ͺ���
void Iec101LinkSend(void)
{
        if (lpIEC101->PSendFrame.byFull == 1)
	{
 
          Serial_Write(2,lpIEC101->PSendFrame.byLinkBuf,lpIEC101->PSendFrame.wFrameLen);
          //if (lpIEC101->wPSendNum >= lpIEC101->PSendFrame.wFrameLen)
          {
            lpIEC101->wPSendNum = 0;
            lpIEC101->byFrameIntval = 0;
            lpIEC101->PSendFrame.byFull = 0;
#if 0
            if ( (lpIEC101->PReFrameType == 0) && (lpIEC101->PReMsgType == 0) )
            {
              lpIEC101->PWindow = 0;
              lpIEC101->PWinTimer = 0;
              lpIEC101->OrgnizeFrame = 0 ;
            }
#else
            if((lpIEC101->PReFrameType == 0xff))
            {
              lpIEC101->OrgnizeFrame = 0 ;
              lpIEC101->PWindow = 0;
            }
            lpIEC101->PWinTimer = 0;
#endif            
          }
	}
}
void OrgnizeIec101Frame(void)
{
	//Ӧ�ò㷢�ʹ�������
	Iec101AppSendPro();
	SearchFirstData();  //����һ�����ݷ�����Ӧ��֡������·֡ǰfulianqiang 2005.10.14
	//��·�㷢�ʹ�������
	Iec101LinkSendPro();
	//��·�㷢�ͺ���
   //	Iec101LinkSend();
}
//��Լ����ʱ����⺯��
void Iec101WatchTime(void)
{
  if(lpIEC101==0)
    return;
  if (lpIEC101->PWindow == 1)
  {
    lpIEC101->byFrameIntval++;
    lpIEC101->PWinTimer++;
  }
  if(3==lpIEC101->UnsolTimeInterval)
  {
    if(lpIEC101->OrgnizeFrame && (lpIEC101->PWinTimer>100))
    {
      if(lpIEC101->PReFrameType!=0xff && lpIEC101->PReFrameType!=0)
      {  
        lpIEC101->PWinTimer = 0;
        lpIEC101->PReFrameType = 0x3;
        lpIEC101->PSendFrame.byFull = 1;
      }
    }
  }
  if (lpIEC101->PWinTimer >= 60000) //��Լ60����ʱ��,���뱣֤�Ͳ�����ʱ,���ٻ���Ϣ�ܹ��������
  {
    lpIEC101->PWindow = 0;
    lpIEC101->PWinTimer = 0;
    lpIEC101->PReFrameType = 0xff;
    lpIEC101->PReMsgType = 0;
    lpIEC101->PSendFrame.byFull = 0;
    lpIEC101->wPSendNum = 0;
  }
}
//101��Լ֡��������
void IEC101Process(void)
{
	//��֡����
	SleaveIec101Frame();
        
	//���ӴӶ�����
	WatchPWindow();

	//��֡����
	OrgnizeIec101Frame();
	Iec101LinkSend();//��·�㷢��
#if 1
	//��Լ��ʱ������
	//Iec101WatchTime();
#endif        

}
//��ʼ����Լ˽�����ݺ���
void InitIEC101Func(u8 ChanNo)
{
    
#if 0 
	WORD i,n;
	lpIEC101=&(lpChanl[ChanNo].ChanlPrivM.IEC101ProtPrivM);	//��ʼ����ͨ����Լ˽������ָ��
	lpIEC101->byCOI = 0;				// ��ʼ��ԭ��:���ص�Դ���غ���
	lpIEC101->FtuSccReset = FALSE;//ͨ����λ��־
	//��ʼ�� SOE �Ķ�ָ��
	lpChanl[ChanNo].ReadSoeC = sHostSoe.bySoeWrite;
	//���SOE���е�ң�ű�λ������
	lpIEC101->YxChangeC = sHostSoe.bySoeWrite;	
	lpIEC101->PWindow = 0;
	lpIEC101->PWinTimer = 0;
	lpIEC101->wRecvNum = 0;
	lpIEC101->wRecvLen = BUFLENGTH;
	lpIEC101->wPSendNum = 0;
	lpIEC101->byFrameIntval = 0;
	lpIEC101->firstData = nofirstdata;
	lpIEC101->TypeProtocol = pChannelPara[ChanNo].wChanPara2;
	if(lpIEC101->TypeProtocol == 0)	   //���ѡ��97���101������Ĳ�����Чfulianqiang 2005.9.2
	{
	   lpIEC101->TypeLinkAdd = 1;
	   lpIEC101->TypeSeReason = 1;
	   lpIEC101->TypeCmmAdd = 1;
	   lpIEC101->TypeInfAdd = 2;
	}
	else				   //2002���101��Ҫ��Ĳ���
	{
		if(pChannelPara[ChanNo].wChanPara3==2)
	   	   lpIEC101->TypeLinkAdd = pChannelPara[ChanNo].wChanPara3; //��"ͨ������3"����վ��ַ����fu 2005.9.1
		else
		   lpIEC101->TypeLinkAdd = 1;
		if( pChannelPara[ChanNo].wChanPara4 ==2)
		   lpIEC101->TypeSeReason = pChannelPara[ChanNo].wChanPara4; //��"ͨ������4"���ô���ԭ������fu 2005.9.1
		else
		   lpIEC101->TypeSeReason = 1;
		if(pChannelPara[ChanNo].wChanPara5==2)
		   lpIEC101->TypeCmmAdd = pChannelPara[ChanNo].wChanPara5; //��"ͨ������5"����Ӧ�÷���Ԫ��ַ����fu 2005.9.1
		else
		   lpIEC101->TypeCmmAdd = 1;
		if( pChannelPara[ChanNo].wChanPara6==3 ||  pChannelPara[ChanNo].wChanPara6 == 1)  //�������ֽڽ����ڽṹ����Ϣ�����ַ������������²�ͬ��Ϣ�����ַ�������ĿΪ65536����������λλ�飩
		   lpIEC101->TypeInfAdd = pChannelPara[ChanNo].wChanPara6; //��"ͨ������6"������Ϣ���ַ����fu 2005.9.1
		else
		   lpIEC101->TypeInfAdd = 2;
	}
	lpIEC101->wLinkAdd.Word =pChannelPara[ChanNo].wChanPara0;	//��"ͨ������0"����վ��ַ
	lpIEC101->wCmmAdd.Word = lpIEC101->wLinkAdd.Word;

	if(pChannelPara[ChanNo].wChanPara7 > 0)
	       lpIEC101->PC56Time2a = TRUE;
	else
	       lpIEC101->PC56Time2a = FALSE;

	lpIEC101->initstatus = notinit;
	lpIEC101->haveset = FALSE;
	lpIEC101->PfcbC = 0;
	lpIEC101->OrgnizeFrame = 0 ;
	lpIEC101->Pfcv = 0;
	lpIEC101->Pacd = 0;
	lpIEC101->Pdfc = 0;
	lpIEC101->PRecvFrame.byFull = 0;
	lpIEC101->PReAppLayer.byFull = 0;
	lpIEC101->PSeAppLayer.byFull = 0;
	lpIEC101->PSendFrame.byFull = 0;
	lpIEC101->PSeAppLayer.lpByBuf = lpIEC101->PSendFrame.byLinkBuf + 5 + lpIEC101->TypeLinkAdd;
	lpIEC101->PReFrameType = 0;
	lpIEC101->PReMsgType = 0;
	lpIEC101->dwReadAd.Dword = 0;	//fulianqiang 2005.9.2
	lpIEC101->WaitYkXzRet = 0;
	lpIEC101->WaitYkZxRet = 0;
	lpIEC101->SendYkZxAck = 0;
	lpIEC101->byYKWaitTimes=0;	//ң�ط�У�ȴ���������
	lpIEC101->nStartYcOver = 0;
	for (i=0;i<YCSendTabSize;i++)
	{
		lpIEC101->nLastYcVal[i]=GetYc(i);
	}
   	if (pChannelPara[ChanNo].wChanPara1)	//ң��Խ��ǧ�ֱ�
	{
		if (pChannelPara[ChanNo].wChanPara1 < 1000)
			lpIEC101->byRange = (u8)pChannelPara[ChanNo].wChanPara1;
		else
			lpIEC101->byRange = 1;
	}
	else
	{ 
		lpIEC101->byRange = 1;
	} 
	//ʵ�ʷ�������ʼ��
	if (pChannelPara[ChanNo].wSendYCNum<=YCSendTabSize)	//�жϲ�����Ч��
		lpIEC101->SendYcN=pChannelPara[ChanNo].wSendYCNum;//��ʼ������ң����
	else
		lpIEC101->SendYcN=YCSendTabSize;
	if (pChannelPara[ChanNo].wSendYXNum<=YXSendTabSize)
		lpIEC101->SendYxN=pChannelPara[ChanNo].wSendYXNum;//��ʼ������ң����
	else
		lpIEC101->SendYxN=YXSendTabSize;
	if (pChannelPara[ChanNo].wSendYMNum<=YMSendTabSize)
		lpIEC101->SendDdN=pChannelPara[ChanNo].wSendYMNum;//��ʼ�����͵����
	else
		lpIEC101->SendDdN=YMSendTabSize;
	lpIEC101->SendYkN=IEC101_YKNUM;
	//��ʼ����Ҫ���͵�ң��֡��,����һ֡�İ�һ֡����
	lpIEC101->YcFN=(u8)((lpIEC101->SendYcN+IEC101_YCNPF-1)/IEC101_YCNPF);
	//��ʼ����Ҫ���͵�ң��֡��,����һ֡�İ�һ֡����
	lpIEC101->YxFN=(u8)((lpIEC101->SendYxN+IEC101_YXNPF-1)/IEC101_YXNPF);
	//��ʼ����Ҫ���͵ĵ��֡��,����һ֡�İ�һ֡����
	lpIEC101->DdFN=(u8)((lpIEC101->SendDdN+IEC101_DDNPF-1)/IEC101_DDNPF);
	for(i=0;i<IEC101_YCNUM/IEC101_YCNPF;i++)
		lpIEC101->YcNPF[i]=0;
	for(i=0;i<IEC101_YXNUM/IEC101_YXNPF;i++)
		lpIEC101->YxNPF[i]=0;
	for(i=0;i<IEC101_DDNUM/IEC101_DDNPF;i++)
		lpIEC101->DdNPF[i]=0;
	//��ʼ��ÿ֡ң����,���һ֡ң����Ϊʵ����Ҫ���͵�ң����
	n=lpIEC101->YcFN;
	for(i=1;i<=n;i++)
		lpIEC101->YcNPF[i-1]=IEC101_YCNPF;
	if ((n>0)&&(lpIEC101->SendYcN%IEC101_YCNPF!=0))
		lpIEC101->YcNPF[n-1]=lpIEC101->SendYcN%IEC101_YCNPF;
	//��ʼ��ÿ֡ң����,���һ֡ң����Ϊʵ����Ҫ���͵�ң����
	n=lpIEC101->YxFN;
	for (i=1;i<=n;i++)
		lpIEC101->YxNPF[i-1]=IEC101_YXNPF;
	if ((n>0)&&(lpIEC101->SendYxN%IEC101_YXNPF!=0))
		lpIEC101->YxNPF[n-1]=lpIEC101->SendYxN%IEC101_YXNPF;
	//��ʼ��ÿ֡�����,���һ֡�����Ϊʵ����Ҫ���͵ĵ����
	n=lpIEC101->DdFN;
	for (i=1;i<=n;i++)
		lpIEC101->DdNPF[i-1]=IEC101_DDNPF;
	if ((n>0)&&(lpIEC101->SendDdN%IEC101_DDNPF!=0))
		lpIEC101->DdNPF[n-1]=lpIEC101->SendDdN%IEC101_DDNPF;
    //������ʼ����
     //	if(pChannelPara[ChanNo].wChanPara8==0)	      //��������ʱ����
     //		lpIEC101->UnsolTimeInterval=10;
     //	else
    //	        lpIEC101->UnsolTimeInterval = pChannelPara[ChanNo].wChanPara8; 
       lpIEC101->UnsolTimeInterval = pChannelPara[ChanNo].wChanPara8; //Ϊ3ʱ���ù����ٻ���ʽ������Ϊ�����Ϸ�ʽ
#endif      
}
//��ʼ��IEC101��Լ�����ṹ
void InitIEC101Prot(void)
{
    int i;
#if 0    
    lpIEC101->TypeLinkAdd=1;
    lpIEC101->TypeCmmAdd=1;
    lpIEC101->TypeInfAdd=1;
    lpIEC101->TypeSeReason=1;
#else
    lpIEC101->TypeCmmAdd=2;
    lpIEC101->TypeLinkAdd=2;
    lpIEC101->TypeInfAdd=2;
    lpIEC101->TypeSeReason=2;
#endif    
    lpIEC101->wLinkAdd.Word=1;
    lpIEC101->initstatus = notinit;
     lpIEC101->haveset = FALSE;
    lpIEC101->FlagPingH = 1;
    lpIEC101->UnsolTimeInterval=3;
    lpIEC101->firstData = nofirstdata;
   
    
    lpIEC101->TypeProtocol=1;
    lpIEC101->PSeAppLayer.lpByBuf = lpIEC101->PSendFrame.byLinkBuf + 5 + lpIEC101->TypeLinkAdd;
    lpIEC101->YcFN = 8;
    for(i=0;i<20;++i)
    {
      lpIEC101->YcNPF[i]=23;//23;
    }
    lpIEC101->DdFN = 16;
    for(i=0;i<20;++i)
    {
      lpIEC101->DdNPF[i]=8;
    }
#if 0 
	ProtocolDisp[IEC101ProtNo].ProtocolType=IEC101ProtNo;	//��Լ����
	ProtocolDisp[IEC101ProtNo].RecvBuffLen=RECEBUFSIZE; 	//���ջ���������
	ProtocolDisp[IEC101ProtNo].TranBuffLen=TRANSBUFSIZE;	//���ͻ���������
	ProtocolDisp[IEC101ProtNo].SyncByte[0]=0xFF;	 		//ͬ����
	ProtocolDisp[IEC101ProtNo].SyncByte[1]=0x68;	 		//ͬ����
	ProtocolDisp[IEC101ProtNo].WordWidth=1;		 			//�ֿ���
	ProtocolDisp[IEC101ProtNo].FrameProcess=IEC101Process;	//֡��������ָ��
	ProtocolDisp[IEC101ProtNo].InitProcess=InitIEC101Func; 	//��ʼ����������ָ��
#endif        
}

void SetACD(void)
{
  lpIEC101->Pacd = 1; //zzl test
}