#ifndef _SYWG_H_
#define _SYWG_H_

#include "stdafx.h"
//typedef unsigned char       BYTE;
//typedef unsigned short      WORD;
//typedef unsigned long       DWORD;


#define EXTENSION_BUF_LEN 1024

#define BLOCK_TYPE_RESULT 3
#define BLOCK_TYPE_RETURN 2

#define ERR_CODE_NETWORK 1000
#define ERR_CODE_DES 1001
#define ERR_CODE_PACKAGE 1002

// �۸�
typedef long	 SWISmallMoney;



#pragma   pack(1)


/*
7�������8�ֽ�SQLMONEY����ʾ���ڲ���ʾΪ__int64����ʮ���ƺ���λ������λС��, �磤12.32  ��ʾΪ 123200
typedef struct _SWIMoney    // Same as the type CURRENCY in VB
{
		DWORD lo_value;
		long  hi_value;
} SWIMoney;

ע�⣺ �Կ���ʽ�����ί�л��ѯ���ع�Ʊ����(fund_amount��ʹ��SWIMoney����ʾ)����ʮ���ƺ�4λ����4λС������1000000.12��ʾΪ10000001200��
*/
typedef struct _SWIMoney    // Same as the type CURRENCY in VB

{

         DWORD lo_value;

         long  hi_value;

#ifdef WIN32

         operator double()

         {

                   return 0.0001 * *((__int64*)this);

         }

 

         _SWIMoney& operator = (double v)

         {

                   union{ __int64 i64;

                            _SWIMoney  mny;

                   } x;

                   x.i64 = (__int64)(10000.0 * v + 0.1 * (v>=0?1:-1));

                   lo_value = x.mny.lo_value;

                   hi_value = x.mny.hi_value;

                   return *this;

         }

#endif

}SWIMoney, FAR* PSWIMoney;


// ��Ϣͷ
struct SWI_BlockHead
{
	WORD block_size;	// ������Ϣ���ֽ���
	WORD crc;			// ���¸��ֶ�block_type�� (������block_size��crc�ֶα���) ��CRCУ����
	BYTE block_type;	// ������  	1 �C ����         ��SWI_???Request��
						//         	2 �C ����״̬     ��SWI_???Return��
						//          	3 �C ��ѯ�����   ��SWI_???Result��
						//          	5 �C ȡ����ѯ
						//          	4 �C ������Ϣ
						//          	5 �C ����Ի�
						//          	6 �C ��������(������Կ)
						//          	7 �C ����Ӧ��(������Կ)
						//          	8 �C ��չ������Ϣ
						//          	9 �C ��չӦ����Ϣ
	BYTE reserved1;    	// ������������0
	WORD function_no;  	// ���ܺ�
	long cn_id;      	// �������Ӻ�
	WORD reserved2;  	// �������ڲ�ʹ�ã�ԭӪҵ����
	WORD dest_dpt;    	// Ŀ��Ӫҵ�����
};


struct SWI_ConnectRequest
{
	SWI_BlockHead head;    	// ��Ϣͷ block_type == 6
/*
��Ϣͷ�У�
block_type	6 �C ��������(������Կ)
		function_no  	����
		cn_id      	0
dest_dpt    	����
*/
	WORD method;     	   	// �ͻ��˵ļ��ܷ�ʽ
	char entrust_method;   	// ί�з�ʽ���������ֵ�˵����
	BYTE return_cp;        	// �ͻ���ϣ���ķ��ؽṹ�������ֶεĴ���ҳ
						     		//   	0 -- MBCS  ��CP936�������ģ�
						     		//   	1 -- Unicode(Java�ͻ���Ҫ)
	BYTE network[4];      	// �ͻ���Novell���κ�
	BYTE address[6];      	// �ͻ���������ַ��Novell�ڵ��ַ��
	WORD client_major_ver;	// �ͻ���Э��ӿ����汾�ţ����ڰ汾Э�̣�
	WORD client_minor_ver;// �ͻ���Э��ӿڴΰ汾�ţ����ڰ汾Э�̣�
	WORD key_length;      // RSA��Կ���ȣ���method 1, key_length=0��
};

struct SWI_ConnectResponse
{
	SWI_BlockHead  head;    	// ��Ϣͷ block_type == 7
/*
block_type	7
		function_no  	����
cn_id      	���䵽�����ӱ�ʶ���ͻ���Ӧ��������ӱ�ʶ����������������  �е���Ӧ�ֶ�
dest_dpt    	����Ӫҵ�����
*/
    WORD return_value;		// ����״̬
	WORD method;             // ����ͬ��ļ��ܷ�ʽ
	char department_name[60];// Ӫҵ�����ƣ�MBCS/Unicode��
	WORD key_length;         // DES��Կ���ȣ�key_length=8��
	BYTE des_key[8];         // DES��Կ(���á�ExpressT����Ϊ��Կdes����)
};

// ���ܺ�901
struct SWI_ErrorMsgRequest
{
    struct SWI_BlockHead head;  // function_no==0x901, block_type==1
    long    error_no;           // �������
};

struct SWI_ErrorMsgReturn
{
    struct SWI_BlockHead head;	//function_no==0x901,block_type==2
    long    return_status;      // ����״̬
    char    error_msg[40];      // ������Ϣ��MBCS/Unicode��
};
// ���ܺ�901

// ���ܺ�0x101
struct SWI_OpenAccountRequest
{
    struct SWI_BlockHead head;	    // function_no==0x101, block_type == 1
    char   account_type;     	    // �ͻ��ʺ�����
    char   account[16];      	    // �ͻ��ʺ�
    char   pwd[8];           	    // �������루��Ϊ����Ա��½����Ϊ����Ա���룩
    short  op_code;                 // ����Ա����
    unsigned  long   flag;          //"��������"�жϱ�־λ���,ÿһ��������λ��Ӧһ��"��������",ȱʡֵΪ0��ʾ���ж��κ�"��������"
    char   productno [7];		    //��Ʒ��Ϣ��ţ�5λ�����������Ĳ�Ʒ����+2λ��
    //��Ʒ���루��7λ�ַ�����ȱʡֵΪ�ա�
    char   note[30];		        //��ע��Ŀǰ�����ڿͻ�ί��ʱ�����Χ�ͻ���½��MAC��ַ ��IP ��ַ��绰�������Ϣ��
};

struct SWI_OpenAccountReturn
{
    struct SWI_BlockHead head; 	//  function_no==0x101
    long    return_status;      //  ����״̬
    BYTE    flag;				//  (�������ֶ�)�������ر�־��0����Ĭ�ϣ���1����
    //  ���ͻ��ŵ�¼����1ʱ,�����ڡ�����Լ����
	char    last_login_date[9];  // �ϴε�¼�����ڣ���ʽΪ��YYYYMMDD
	char    last_login_time[9];  // �ϴε�¼��ʱ�䣬��ʽΪ��HH:MM:SS
	char    last_full_note[60];  // �ϴεĵ�½��Ϣ���μ�ע������5��˵��
	char    reserved_auth_info[20];    // �ͻ�Ԥ����֤��Ϣ
};
// ���ܺ�0x101


// ���ܺ�0x111
struct SWI_AccountLoginRequest
{
    struct SWI_BlockHead head;	// function_no==0x111, block_type == 1
    char   account_type;     	// �ͻ��ʺ����ͣ��������ֵ�˵����
    char   account[16];      	// �ͻ��ʺ�
    char   pwd[8];           	// ��������
    unsigned  long   flag;      //"��������"�жϱ�־λ���,ÿһ��������λ��Ӧһ��"��������",ȱʡֵΪ0��ʾ���ж��κ�"��������"
    char   productno [7];		//��Ʒ��Ϣ��ţ�5λ�����������Ĳ�Ʒ����+2λ��
    //��Ʒ���루��7λ�ַ�����ȱʡֵΪ�ա�
    char   note[30];	        //��ע��Ŀǰ�����ڿͻ�ί��ʱ�����Χ�ͻ���½��MAC��ַ ��IP ��ַ��绰�������Ϣ��

};

struct SWI_AccountLoginResult
{
    struct SWI_BlockHead head;     	// function==0x111,block_type==3
    WORD   row_no;          	    // ��¼�ţ�0xFFFF��ʾ��¼������
    long   bankbook_number;         // �ʽ��ʺ�
    char   account_type;  			// �ʺ����ͣ��������ֵ�, �����ʽ��ʺš�0����
    char   security_account[16];	// ֤ȯ�ʺ�
};


struct SWI_AccountLoginReturn
{
    struct SWI_BlockHead head;		// function_no = 0x111,block_type==2
    long   return_status;     		// ����״̬
    long    bankbook_number;     	// �ʽ��ʺ�
    BYTE    account_status;      	// �ʺ�״̬���������ֵ�˵����
    char    name[20];            	// �ͻ�������MBCS/Unicode��
    char    id[19];      			// ���֤��
    BYTE    card_version;          	//�ſ��汾��
    char    customer_flag[16];      //�ͻ����Ի���Ϣ��־
    char	Cust_flag[32];		    //�ͻ�Ȩ�ޱ�־���ֶ��������R��������������ȯ��
    char	Cust_risk_type;		    //�ͻ�����������𣨼������ֵ�˵����
	short   depart_number;			//Ӫҵ�����루4λ��
	char    last_login_date[9];     // �ϴε�¼�����ڣ���ʽΪ��YYYYMMDD
	char    last_login_time[9];     // �ϴε�¼��ʱ�䣬��ʽΪ��HH:MM:SS
	char    last_full_note[60];     // �ϴεĵ�½��Ϣ���μ�0x101ע������5��˵��
	char    reserved_auth_info[20];     // �ͻ�Ԥ����֤��Ϣ
};
// ���ܺ�0x111


struct SWI_AddOTCEntrustRequest
{
    struct SWI_BlockHead head;	// function_no==0x2601, block_type == 1
    
    char    account[16];      // ��¼�ͻ��ʽ��˺�
	char    bs_type;          // �������
                          // ��1�� ----- ����
	// ��2�� ----- ����
							  // ��7�� ----- �Ϲ�
	// ��c�� ----- �깺
	// ��d�� ----- ���
	char    ta_code[8];       // �Ǽǻ�������
		char    security_code[7]; // ��Ʒ����
		SWIMoney	   fund_amount;  //ί������(֧�ֿ���ʽ����С���ݶ�)
		SWISmallMoney price;      // ί�м۸�SWISmallMoney�������ֵ�˵������ͬ��
	short   effect_days;      // ��Ч����
	SWIMoney   apply_amount;  //������ 
	BYTE    mark;             //�Ƿ�������� 1Ϊ�������,0Ϊ������ ȷʡΪ0
	char 	MarketOrder_type[2]; // �۸����� Ĭ��Ϊ��
	//��11������  �ɽ��걨
	//��12������  �����걨
	char 	match_type;     //  ����Լ��
								  //  ��0��: �ɲ��ֳɽ�
	//  ��1��: ���ɲ��ֳɽ�
	long 	contract_no;	    //Լ���ţ��۸�����Ϊ��11��ʱ��Ҫ���룩
	char    contact_name[30];  //��ϵ������
	char 	contact_tel [30];   //��ϵ��ʽ
	char    Entrust_sign[10];   //����ǩ��(ȱʡΪ����)


};

struct SWI_AddOTCEntrustReturn
{
	struct SWI_BlockHead head;
	long return_status;   	      	//  ����״̬
	long entrust_sn;      			//  ί�б��
};

// 0x4603
struct SWI_QueryOTCPDEntrustRequest
{
	struct SWI_BlockHead head; 	// function_no == 0x4603 ,block_type==1
	char    account[16];     	// ��¼�ͻ��ʽ��˺�
char    query_type;         // ��ѯ���� 
//��1��---��ǰ��¼ ��ʾ����δ�걨��TAϵͳ��ί�м�¼
//��2��---��ʷ��¼
	long    begin_date;      	// ��ʼ���ڣ����磺yyyymmdd��
	long    end_date;        	// �������ڣ����磺yyyymmdd��
	char    inst_id [21];	    // ��Ʒ���루Ĭ��Ϊ�գ��ձ�ʾȫ����
char    query_flag;         // ��ѯ��ʶ (Ĭ�ϡ�0��������ȫ��ί�� ��
//               ��1����ֻ������Чί�У��ɳ���ί�У���
};

struct SWI_QueryOTCPDEntrustResult
{
	struct SWI_BlockHead head; // function_no==0x4603,block_type==3
	WORD row_no;               // ��¼��,  0xFFFF���������
	long entrust_date;         // ί�����ڣ����磺yyyymmdd��
char entrust_sn[21];       // ί�б��
	char entrust_time[9];      // ί��ʱ�䣨���磺hh:mm:ss��
    char ta_acct[30];          // TA�Ǽ��˻�
	char inst_id [21];	       // ��Ʒ���루Ĭ��Ϊ�գ��ձ�ʾȫ����
	char security_name[32];    // ֤ȯ���ƣ�MBCS/Unicode��
	char bs_type[3];              // ������� �������ֵ�3.3,�������������ͣ�
                           // ��110�� ----- �Ϲ�
// ��111�� ----- �깺
// ��112�� ----- ���
SWIMoney  app_amt;         // ������
SWIMoney  redeem_num;      // ��طݶ�
	char  entrust_status;      // ί��״̬���������ֵ�1.4��
char  cancel_flag;         // ������־ ��0��-��������1��-����
char  orient_app_no[21];   // ԭ������
char  cancelled_flag;      // �ѳ���־ ��0��-δ������ ��1��-�ѳ���
char  can_cancel_flag;     // �ɳ���־ ��0��-���ɳ�������1��-�ɳ���
SWIMoney  cfm_amt;             // ȷ�Ͻ��
SWIMoney  cfm_num;             // ȷ������
};


struct SWI_QueryOTCPDEntrustReturn
{
	struct SWI_BlockHead head; 	// function_no==0x4603,block_type==2
	long   return_status;      	// ����״̬ >0��ʾ����ѯ�ɹ�, <=0��ʾ����ѯʧ��
};

// 0x4605
struct SWI_QueryOTCStockRequest
{
  struct SWI_BlockHead head;     // function_no == 0x4605, block_type==1
  char   account[16];     // ��¼�ͻ��ʽ��˺�
  char   inst_id [21];        // ��Ʒ���루Ĭ��Ϊ�գ��ձ�ʾȫ����
};

struct SWI_QueryOTCStockResult
{
  struct SWI_BlockHead head;  // function_no == 0x4605,block_type==3
WORD row_no;          //  ��¼�� , 
//  0xFFFF -- ���������(�����������	  ��Ч)
    char ta_acct[30];            // TA�Ǽ��˻�
   char inst_id[21];         // ��Ʒ����
  char security_name[32];   // ��Ʒ���ƣ�MBCS/Unicode��
  SWIMoney fund_deposite;    // ��ǰ���
  SWIMoney fund_available;   // ���÷ݶ�
  SWIMoney today_bought_amount;   // ��������(�ɽ�)����
  SWIMoney today_sell_amount;    // �����������ɽ�������	
SWIMoney	 buy_cost;	         // ����ɱ����
	SWIMoney	 market_value;	     // ��ֵ
};


struct SWI_QueryOTCStockReturn
{
struct SWI_BlockHead head; // function_no== 0x4605, block_type==2
  long return_status;     // ����״̬ >0��ʾ����ѯ�ɹ�, <=0��ʾ����ѯʧ��
};

// 0x4606
struct SWI_QueryOTCBusinessRequest
{
	struct SWI_BlockHead head;		// function_no==0x4606,block_type==1
	char    account[16];      		// ��¼�ͻ��ʽ��˺�
	long    begin_date;      	    // ��ʼ���ڣ����磺yyyymmdd��
	long    end_date;             	// �������ڣ����磺yyyymmdd��
};

struct SWI_QueryOTCBusinessResult
{
	struct SWI_BlockHead head;// function_no==0x4606, block_type==3
WORD row_no;       // ��¼�� , 
// 0xFFFF -- ���������(�������������Ч)
char ta_acct[30];         // TA�Ǽ��˻�
	char inst_id[21];       // ��Ʒ����
  char security_name[32];  // ��Ʒ���ƣ�MBCS/Unicode��	
    char bs_type[3];             // ���������������ֵ�1.3
    SWIMoney   done_num;      // �ɽ�����
    SWISmallMoney done_price; // �ɽ��۸�
    SWIMoney   clr_amt;       // ������
SWIMoney   commision;	  // ������
SWIMoney   sf_amt;	      // �ո����
long       clr_date;      // ��������
long       app_date;      // ��������
};


struct SWI_QueryOTCBusinessReturn
{
	struct SWI_BlockHead head;	// function_no==0x4606, block_type==2
	long    return_status;  	// ����״̬ >0��ʾ����ѯ�ɹ�, <=0��ʾ����ѯʧ��
};

// 0x2601

#pragma   pack()

#endif
