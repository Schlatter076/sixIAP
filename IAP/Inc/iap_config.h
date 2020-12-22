#ifndef __IAP_CONFIG_H__
#define __IAP_CONFIG_H__
/* Define if use bkp save flag  -------------------------------*/
#define USE_BKP_SAVE_FLAG     0

/* Define the APP start address -------------------------------*/
#define ApplicationAddress    0x8010000

/* Output printer switch --------------------------------------*/

/* Bootloader command -----------------------------------------*/
#define BootLoader_Version    "sixBT20200713"
//#define CMD_UPDATE_STR        "update"
//#define CMD_UPLOAD_STR        "upload"
//#define CMD_ERASE_STR		  "erase"
//#define CMD_MENU_STR          "menu"
//#define CMD_RUNAPP_STR        "runapp"
//#define CMD_ERROR_STR         "error"
//#define CMD_DISWP_STR         "diswp"//��ֹд����
//#define CMD_setID_STR         "setID"
//#define CMD_getID_STR         "getID"

/**
 * 1.1 �������� �ϵ�ע��
 */
#define REQ_REGISTER  "00"
/**
 * 1.2 �Ӷ����� ע��ʧ��
 */
#define RES_REGISTER_ERROR  "01"
/**
 * 1.3 �Ӷ����� Ҫ����¹̼�
 */
#define RES_REGISTER_AND_UPDATE  "02"
/**
 * 1.4 �Ӷ����� ע��ɹ�
 */
#define RES_REGISTER_SUCCESS  "03"
// ************************** 2.2 �̼����� ****************************//
/**
 * 2.1 �������� ����̼��汾��Ϣ
 */
#define REQ_VERSION  "04"
/**
 * 2.2 �Ӷ����� �汾��Ϣ
 */
#define REQ_CURRENT_VERSION  "05"
/**
 * 2.3 �������� �����N֡�̼�
 */
#define REQ_REQUEST_FRAME  "06"
/**
 * 2.4 �Ӷ����� ��N֡�̼�����
 */
#define RES_RESPONSE_FRAME  "07"
/**
 * 2.5 �������� ��ɹ̼�����
 */
#define REQ_UPDATE_FINSHED  "08"
/**
 * 2.6 �Ӷ����� ��Ӧ�յ��豸��ɸ���
 */
#define RES_UPDATE_FINEHED_REPONSE  "09"
// ************************** 2.3 �ϱ�Ӧ�÷������������(Ԥ��) ****************************//
// ************************** 2.4 �ϱ��쳣 ****************************//
/**
 * 4.1 �������� �ϱ��쳣
 */
#define REQ_ERROR_REPORT  "E0"
/**
 * 4.2 �Ӷ����� ��Ӧ�յ��쳣�ϱ�
 */
#define RES_ERROR_REPORT_RECEIVED  "E1"

#define RES_UPLOAD  "F1"
#define RES_ERASE   "F2"
#define RES_REBOOT  "F3"
#define RES_DIS_WP  "F4"
#define RES_GET_ID  "F5"
#define RES_SET_ID  "F6"
#define REQ_SET_ID  "F7"

/* IAP command------------------------------------------------ */
#define IAP_FLAG_ADDR   0x0803F800

//#define IAP_FLASH_FLAG_ADDR   0x8002800

#define INIT_FLAG_DATA      0xFFFF   //Ĭ�ϱ�־������(��Ƭ�ӵ����)
#define UPDATE_FLAG_DATA        0xEEEE   //���ر�־������
#define UPLOAD_FLAG_DATA        0xDDDD   //�ϴ���־������
#define ERASE_FLAG_DATA         0xCCCC   //������־������
#define APPRUN_FLAG_DATA        0x5A5A   //APP����Ҫ���κδ���ֱ������״̬
#define SET_DeviceID_FLAG_DATA  0xEDED  //д�豸���
#define GET_DeviceID_FLAG_DATA  0xDEDE  //���豸���

/* Define the Flsah area size ---------------------------------*/
#define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
#define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
/* Compute the FLASH upload image size --------------------------*/
#define FLASH_IMAGE_SIZE      (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))

/* The maximum length of the command string -------------------*/
#define CMD_STRING_SIZE       128

#endif
