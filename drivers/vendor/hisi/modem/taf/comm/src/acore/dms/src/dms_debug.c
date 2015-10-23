

/*******************************************************************************
 PROJECT   :
 SUBSYSTEM :
 MODULE    :
 OWNER     :
********************************************************************************/
#include <dms.h>

DMS_DEBUG_INFO_TBL_STRU g_ast_dsm_debug_info_table[EN_SDM_DMS_DEBUG_INFO_MAX];

DMS_DEBUG_AT_SERV_NV_STRU g_dms_debug_atserv_nv_info = {0};

void dms_PrintAtServerNvInfo(VOS_VOID)
{
    vos_printf("Operation return value is %d.\r\n", g_dms_debug_atserv_nv_info.lOperatRet);
    vos_printf("The nv value is %d.\r\n", g_dms_debug_atserv_nv_info.ulNvValue);
}

DMS_DEBUG_INFO_TBL_STRU* dms_DebugGetInfo(VOS_VOID)
{
   return g_ast_dsm_debug_info_table;
}

VOS_VOID dms_Debug(DMS_DEBUG_MSG_ID_ENUM ulType,
                     VOS_UINT32 ulRserved1, VOS_UINT32 ulRserved2, VOS_UINT32 ulRserved3)
{

    g_ast_dsm_debug_info_table[ulType].ulCalledNum += 1;
    g_ast_dsm_debug_info_table[ulType].ulRserved1 = ulRserved1;
    g_ast_dsm_debug_info_table[ulType].ulRserved2 = ulRserved2;
    g_ast_dsm_debug_info_table[ulType].ulRserved3 = ulRserved3;
    g_ast_dsm_debug_info_table[ulType].ulRtcTime  = VOS_GetTick();


}

#if (VOS_OS_VER == VOS_LINUX)

VOS_VOID SDM_DmsVcom(VOS_VOID)
{
    DMS_DEBUG_INFO_TBL_STRU* pstTable;

    /* 如果某个成员在某个类别没有使用可以不打印 */

    pstTable = dms_DebugGetInfo();


    vos_printf("*------------ENABLE-------------------\n");
    vos_printf("USB ENABLE counts  : %d \n USB ENABLE fail counts  : %d \n USB ENABLE fail branch  : %d \n\n",
            pstTable[EN_SDM_DMS_INIT].ulCalledNum,
            pstTable[EN_SDM_DMS_INIT_ERR].ulCalledNum,
            pstTable[EN_SDM_DMS_INIT_ERR].ulRserved3);
    vos_printf("*------------DISABLE-------------------\n");
    vos_printf("USB DISABLE counts  : %d \n USB DISABLE fail counts  : %d \n USB DISABLE fail branch  : %d \n\n",
            pstTable[EN_SDM_DMS_DISABLE].ulCalledNum,
            pstTable[EN_SDM_DMS_DISABLE_ERR].ulCalledNum,
            pstTable[EN_SDM_DMS_DISABLE_ERR].ulRserved3);

    vos_printf("*------------OPEN-------------------\n");
    vos_printf("VCOM1 OPEN counts  : %d \n VCOM1 OPEN fail counts  : %d \n  VCOM1 OPEN fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM1_OPEN].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_OPEN_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_OPEN_ERR].ulRserved1);
    vos_printf("VCOM2 OPEN counts  : %d \n VCOM2 OPEN fail counts  : %d \n  VCOM2 OPEN fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM2_OPEN].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_OPEN_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_OPEN_ERR].ulRserved1);
    vos_printf("VCOM_AT OPEN counts  : %d \n VCOM_AT OPEN fail counts  : %d \n  VCOM_AT OPEN fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM_AT_OPEN].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_OPEN_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_OPEN_ERR].ulRserved1);
    vos_printf("VCOM_CTRL OPEN counts  : %d \n VCOM_CTRL OPEN fail counts  : %d \n  VCOM_CTRL OPEN fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM_CTRL_OPEN].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_OPEN_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_OPEN_ERR].ulRserved1);

    vos_printf("*------------CLOSE-------------------\n");
    vos_printf("VCOM1 CLOSE counts  : %d \n VCOM1 CLOSE fail counts  : %d \n  VCOM1 CLOSE fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM1_CLOSE].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_CLOSE_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_CLOSE_ERR].ulRserved1);
    vos_printf("VCOM2 CLOSE counts  : %d \n VCOM2 CLOSE fail counts  : %d \n  VCOM2 CLOSE fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM2_CLOSE].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_CLOSE_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_CLOSE_ERR].ulRserved1);
    vos_printf("VCOM_AT CLOSE counts  : %d \n VCOM_AT CLOSE fail counts  : %d \n  VCOM_AT CLOSE fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM_AT_CLOSE].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_CLOSE_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_CLOSE_ERR].ulRserved1);
    vos_printf("VCOM_CTRL CLOSE counts  : %d \n VCOM_CTRL CLOSE fail counts  : %d \n  VCOM_CTRL CLOSE fail value : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM_CTRL_CLOSE].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_CLOSE_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_CLOSE_ERR].ulRserved1);

    vos_printf("*------------WRITE_异步写-------------------\n");
    vos_printf("VCOM1 write counts  : %d \n VCOM1 write OK counts  : %d \n  VCOM1 Last write OK len : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM1_WRT].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_WRT_SUCC].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_CLOSE_ERR].ulRserved1);
    vos_printf("VCOM1 write counts  : %d \n VCOM1 write OK counts  : %d \n  VCOM1 Last write OK len : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM2_WRT].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_WRT_SUCC].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_WRT_SUCC].ulRserved1);
    vos_printf("VCOM_AT write counts  : %d \n VCOM_AT write OK counts  : %d \n  VCOM_AT Last write OK len : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM_AT_WRT].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_WRT_SUSS].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_WRT_SUSS].ulRserved1);
     vos_printf("VCOM_CTRL write counts  : %d \n VCOM_CTRL write OK counts  : %d \n  VCOM_CTRL Last write OK len : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM_CTRL_WRT].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_WRT_SUSS].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_WRT_SUSS].ulRserved1);
    vos_printf("*------------WRITE CALL BACK-------------------\n");
    vos_printf("VCOM1 WRITE CALL BACK counts  : %d \n VCOM1 WRITE CALL BACK fail counts  : %d \n  VCOM1 WRITE fail addr : %d\nVCOM1 WRITE fail len :%d\n\n",
        pstTable[EN_SDM_DMS_VCOM1_WRT_CB].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_WRT_CB_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_WRT_CB_ERR].ulRserved1,
        pstTable[EN_SDM_DMS_VCOM1_WRT_CB_ERR].ulRserved2);
    vos_printf("VCOM2 WRITE CALL BACK counts  : %d \n VCOM2 WRITE CALL BACK fail counts  : %d \n  VCOM2 WRITE fail addr : %d\nVCOM2 WRITE fail len :%d\n\n",
        pstTable[EN_SDM_DMS_VCOM2_WRT_CB].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_WRT_CB_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_WRT_CB_ERR].ulRserved1,
        pstTable[EN_SDM_DMS_VCOM2_WRT_CB_ERR].ulRserved2);

     vos_printf("VCOM_AT WRITE CALL BACK counts  : %d \n VCOM_AT WRITE CALL BACK fail counts  : %d \n  VCOM_AT VCOM_AT fail addr : %d\nVCOM2 WRITE fail len :%d\n\n",
        pstTable[EN_SDM_DMS_VCOM_AT_WRT_CB].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_WRT_CB_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_WRT_CB_ERR].ulRserved1,
        pstTable[EN_SDM_DMS_VCOM_AT_WRT_CB_ERR].ulRserved2);

    vos_printf("VCOM_CTRL WRITE CALL BACK counts  : %d \n VCOM_CTRL WRITE CALL BACK fail counts  : %d \n  VCOM_CTRL VCOM_AT fail addr : %d\nVCOM2 WRITE fail len :%d\n\n",
        pstTable[EN_SDM_DMS_VCOM_CTRL_WRT_CB].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_WRT_CB_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_CTRL_WRT_CB_ERR].ulRserved1,
        pstTable[EN_SDM_DMS_VCOM_CTRL_WRT_CB_ERR].ulRserved2);

    vos_printf("*------------EVT CALL BACK-------------------\n");
    vos_printf("VCOM1 EVT CALL BACK counts  : %d \n VCOM1 EVT  type : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM1_EVT].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM1_EVT].ulRserved1);
    vos_printf("VCOM2 EVT CALL BACK counts  : %d \n VCOM2 EVT  type : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM2_EVT].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM2_EVT].ulRserved1);
    vos_printf("VCOM_AT EVT CALL BACK counts  : %d \n VCOM_AT EVT  type : %d\n\n",
        pstTable[EN_SDM_DMS_VCOM_AT_EVT].ulCalledNum,
        pstTable[EN_SDM_DMS_VCOM_AT_EVT].ulRserved1);

    vos_printf("*------------NCM SEND TO DRV INFO------------------\n");
    vos_printf("发送到驱动次数   : %d\n 发送buf: %x\n 发送长度:%d\n 驱动返回成功次数  : %d \n\n",
        pstTable[EN_SDM_DMS_NCM_SEND_TO_DRV].ulCalledNum,
        pstTable[EN_SDM_DMS_NCM_SEND_TO_DRV].ulRserved1,
        pstTable[EN_SDM_DMS_NCM_SEND_TO_DRV].ulRserved2,
        pstTable[EN_SDM_DMS_NCM_SEND_TO_DRV_SUCC].ulCalledNum);

}

VOS_VOID SDM_UDI_READ(VOS_VOID)
{
     DMS_DEBUG_INFO_TBL_STRU* pstTable;

    /* 如果某个成员在某个类别没有使用可以不打印 */

    pstTable = dms_DebugGetInfo();


    vos_printf("*------------UDI PRE READ FROM BSP----------------\n");
    vos_printf("Times                    : %d \n", pstTable[EN_SDM_DMS_UDI_READ_START].ulCalledNum);
    vos_printf("slReadlen                : %d \n",  pstTable[EN_SDM_DMS_UDI_READ_START].ulRserved1);
    vos_printf("usToReadSize             : %d \n",  pstTable[EN_SDM_DMS_UDI_READ_START].ulRserved2);
    vos_printf("ucRedBuf                 : %d \n",  pstTable[EN_SDM_DMS_UDI_READ_START].ulRserved3);
    vos_printf("RTC Time                 : %d \n\n", pstTable[EN_SDM_DMS_UDI_READ_START].ulRtcTime);

    vos_printf("*------------UDIREAD FROM BSP----------------\n");
    vos_printf("Times                    : %d \n", pstTable[EN_SDM_DMS_UDI_READ_END].ulCalledNum);
    vos_printf("slReadlen                : %d \n",  pstTable[EN_SDM_DMS_UDI_READ_END].ulRserved1);
    vos_printf("usToReadSize             : %d \n",  pstTable[EN_SDM_DMS_UDI_READ_END].ulRserved2);
    vos_printf("ucRedBuf                 : %d \n",  pstTable[EN_SDM_DMS_UDI_READ_END].ulRserved3);
    vos_printf("RTC Time                 : %d \n\n", pstTable[EN_SDM_DMS_UDI_READ_END].ulRtcTime);
}

VOS_VOID SDM_DmsSocket(VOS_VOID)
{
     DMS_DEBUG_INFO_TBL_STRU* pstTable;

    /* 如果某个成员在某个类别没有使用可以不打印 */

    pstTable = dms_DebugGetInfo();

    vos_printf("*------------DMS SOCKET_RECV-------------------\n");
    vos_printf("accept  call counts  : %d \n accept  socket : %d \n  RECV Fail counts : %d\n RECV Fail return : %d\n \n",
        pstTable[EN_SDM_DMS_SOCKET_ACP].ulCalledNum,
        pstTable[EN_SDM_DMS_SOCKET_ACP].ulRserved1,
         pstTable[EN_SDM_DMS_SOCKET_RECV].ulCalledNum,
        pstTable[EN_SDM_DMS_SOCKET_RECV].ulRserved1);

    vos_printf("*------------DMS SOCKET_CLOSE _SEND-------------------\n");
    vos_printf("close socket counts : %d\n close socket ID :%d\nclose socket fail counts  :%d\nclose socket fail return  :%d\nsend fail counts : %d\n send fail return : %d\n\n",
         pstTable[EN_SDM_DMS_SOCKET_CLOSE].ulCalledNum,
        pstTable[EN_SDM_DMS_SOCKET_CLOSE].ulRserved1,
        pstTable[EN_SDM_DMS_SOCKET_CLOSE_FAIL].ulCalledNum,
        pstTable[EN_SDM_DMS_SOCKET_CLOSE_FAIL].ulRserved2);


    vos_printf("*------------DMS SOCKET_RECON-------------------\n");
    vos_printf("Recon Succ counts  : %d \n New Socket ID  : %d \n  Recon Fail counts : %d\n return Value : %d\n \n",
        pstTable[EN_SDM_DMS_TCP_RECONNECT].ulCalledNum,
        pstTable[EN_SDM_DMS_TCP_RECONNECT].ulRserved1,
        pstTable[EN_SDM_DMS_TCP_RECONNECT_ERR].ulCalledNum,
        pstTable[EN_SDM_DMS_TCP_RECONNECT_ERR].ulRserved2);

    vos_printf("*------------DMS SOCKET-------------------\n");

    vos_printf("dms_TcpSendData  call counts  : %d \n chan id : %d \n  buffer len : %d\n send ok branch : %d\n \n",
    pstTable[EN_SDM_DMS_SOCKET_SEND].ulCalledNum,
    pstTable[EN_SDM_DMS_SOCKET_SEND].ulRserved1,
    pstTable[EN_SDM_DMS_SOCKET_SEND].ulRserved2);

    vos_printf("dms_TcpSendData  call counts  : %d \n chan id : %d \n  buffer len : %d\n send fail branch : %d\n \n",
    pstTable[EN_SDM_DMS_SOCKET_SEND_ERROR].ulCalledNum,
    pstTable[EN_SDM_DMS_SOCKET_SEND_ERROR].ulRserved1,
    pstTable[EN_SDM_DMS_SOCKET_SEND_ERROR].ulRserved2);


    vos_printf("dms_findWholePacketSend  call counts  : %d \n chan id : %d \n  buffer len : %d\n send ok branch : %d\n \n",
    pstTable[EN_SDM_DMS_DECODE_PACKET_SEND].ulCalledNum,
    pstTable[EN_SDM_DMS_DECODE_PACKET_SEND].ulRserved1,
    pstTable[EN_SDM_DMS_DECODE_PACKET_SEND].ulRserved2);

    vos_printf("dms_findWholePacketSend  call counts  : %d \n chan id : %d \n  buffer len : %d\n send Fail branch : %d\n \n",
    pstTable[EN_SDM_DMS_DECODE_PACKET_SEND_FAIL].ulCalledNum,
    pstTable[EN_SDM_DMS_DECODE_PACKET_SEND_FAIL].ulRserved1,
    pstTable[EN_SDM_DMS_DECODE_PACKET_SEND_FAIL].ulRserved2);
}

#endif


