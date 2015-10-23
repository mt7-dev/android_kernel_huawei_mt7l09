/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  emmcNormal.c
*
*   作    者 :  wangzhongshun
*
*   描    述 :  normal工作模式
*
*   修改记录 :  2011年6月15日  v1.00  wangzhongshun  创建
*
*************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <string.h>

#include "secBoot.h"
#include "OnChipRom.h"
#include "emmcConfig.h"
#include "emmcErrcode.h"
#include "emmcIp.h"
#include "emmcIdmac.h"
#include "emmcNormal.h"
#include "emmcMain.h"
#include "ioinit.h"
extern UINT32 emmc_form_send_cmd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 cmd_id, UINT32 cmd_arg,
                       UINT8 *pucCmdRespBuff, UINT32 flags);
extern BOOL emmc_get_card_present(UINT32 slot);
extern UINT32  read_write_blkdata(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulBlkOfs, UINT32 ulSize,
                            UINT8  *pucDataBuff,
                            UINT8  *pucCmdRespBuff,
                            UINT8  *pucErrRespBuff,
                            UINT32 ulFlags);
extern UINT32 emmc_process_csd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, BOOL bMmcType);
extern UINT32 emmc_read_bootloader_boot_mode(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulAddress);
extern CARD_TYPE_E emmc_identify_card_type(UINT32 slot,EMMC_GLOBAL_VAR_S *pstGlbVar);
extern UINT32 emmc_check_transtate(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar);
extern UINT32 emmc_read_write_fifo(UINT32 ulByteCount, UINT8 *pucDataBuff, BOOL bWrite);
extern UINT32  emmc_send_command(EMMC_GLOBAL_VAR_S *pstGlbVar, COMMAND_INFO_S *pstCmdInfo);
//extern UINT32 emmc_card_power(BOOL bPowerOn);
extern UINT32 emmc_idmac_init(EMMC_GLOBAL_VAR_S *pstGlbVar);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
* 函 数 名  : emmc_wait_data0_ready
*
* 功能描述  : wait data0 de-assert
*
* 输入参数  : VOID
* 输出参数  : 无
*
* 返 回 值  : ERRNOERROR        :成功
*             ERRRETRYTIMEOUT   :失败

*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_wait_data0_ready(VOID)
{
    TIMER0_VAR_DECLAR;
    volatile UINT32  ulStatus = 0;

    TIMER0_INIT_TIMEOUT(EMMC_DATA0_READY_TIMEOUT);
    do
    {
        ulStatus = INREG32(EMMC_REG_STATUS);

        if(TIMER0_IS_TIMEOUT())
        {
            return ERRRETRYTIMEOUT;
        }
    }while(ulStatus & 0x200); /* Bit 9 is the inverted of Data0. 1 means card data busy */

    /* the D0 has been go to high(ready) */
    return ERRNOERROR;
}

/*****************************************************************************
* 函 数 名  : emmc_data_event_error_handler
*
* 功能描述  : 处理数据命令相关的事件
*
* 输入参数  : UINT32 ulIntStatus
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
void emmc_data_event_error_handler(EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulIntStatus)
{
    UINT32  ulReadBytes = 0 ;

    /*
        Respond to data transfer related errors ie:
        Data timeout,CRC,End bit error,FIFO overrun/underrun.(bit 15,11,10,9,7)
    */
    if(0 == (ulIntStatus & EMMC_INT_BIT_DATA_OR_FIFO_ERROR)) /* 0x00008E80 */
    {
        /* Check if(FIFO threshold reached)(bit4,5) */
        if( ulIntStatus & EMMC_INT_BIT_RXDR )/* Rx_FIFO_datareq */
        {
            /* print_info( "rxd\r\n" ); */
            /*
                Try read data from FIFO
                If 0 bytes to be read, do nothing. Mistimed interrupt.
            */
            if(pstGlbVar->stDatCmdInfo.ulRemainedBytes)
            {
                ulReadBytes = emmc_read_write_fifo(pstGlbVar->stDatCmdInfo.ulRemainedBytes,
                    (pstGlbVar->stDatCmdInfo.pucCurrDataBuff + pstGlbVar->stDatCmdInfo.ulByteCount - pstGlbVar->stDatCmdInfo.ulRemainedBytes),
                    FALSE);

                pstGlbVar->stDatCmdInfo.ulRemainedBytes -= ulReadBytes;
            }
        }
        else if(ulIntStatus & EMMC_INT_BIT_TXDR) /* Tx_FIFO_datareq */
        {
            if(pstGlbVar->stDatCmdInfo.ulRemainedBytes)
            {
                ulReadBytes = emmc_read_write_fifo(pstGlbVar->stDatCmdInfo.ulRemainedBytes,
                    (pstGlbVar->stDatCmdInfo.pucCurrDataBuff + pstGlbVar->stDatCmdInfo.ulByteCount - pstGlbVar->stDatCmdInfo.ulRemainedBytes),
                    TRUE);
                pstGlbVar->stDatCmdInfo.ulRemainedBytes -= ulReadBytes;
            }
        }
        else
        {
            /* nothing to do */
        }
    }
    else
    {
        pstGlbVar->stDatCmdInfo.ulDataErrStatus |= (ulIntStatus&EMMC_INT_BIT_DATA_OR_FIFO_ERROR);
    }
}

/*****************************************************************************
* 函 数 名  : emmc_data_event_dto_handler
*
* 功能描述  : 处理数据命令相关的事件(DTO)
*
* 输入参数  : UINT32 ulIntStatus  :interrupt status
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
void emmc_data_event_dto_handler(EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulIntStatus)
{
    UINT32  ulReadBytes = 0 ;

    /*
        Data transfer over has to be entertained even if there is any
        error during data transfer or abort is sent.
    */
    /* Check if(Data Transfer Over) (bit3) */
    if(ulIntStatus & EMMC_INT_BIT_DTO)
    {
        /* print_info( "DTO\r\n"); */

        /* If 0 bytes to be written,do nothing.Mistimed interrupt. */
        if(pstGlbVar->stDatCmdInfo.ulRemainedBytes != 0)
        {
            /* Here tDatCmdInfo.dwRemainedBytes will never exceed FIFO depth. */
            ulReadBytes = emmc_read_write_fifo(pstGlbVar->stDatCmdInfo.ulRemainedBytes,
                    (pstGlbVar->stDatCmdInfo.pucCurrDataBuff + pstGlbVar->stDatCmdInfo.ulByteCount - pstGlbVar->stDatCmdInfo.ulRemainedBytes),
                    (BOOL)pstGlbVar->stDatCmdInfo.ulWrite);

            pstGlbVar->stDatCmdInfo.ulRemainedBytes -= ulReadBytes;

            /*  If byte count is not in multiples of FIFO_width,
                then, one extra read/write is required.
                DONOT use while loop.Or else,in case of error,
                FIFO may be empty,data transfer over and remained bytes
                will never become 0.Driver will hang.
            */
            if(pstGlbVar->stDatCmdInfo.ulRemainedBytes)
            {
                ulReadBytes = emmc_read_write_fifo(pstGlbVar->stDatCmdInfo.ulRemainedBytes,
                        (pstGlbVar->stDatCmdInfo.pucCurrDataBuff + pstGlbVar->stDatCmdInfo.ulByteCount - pstGlbVar->stDatCmdInfo.ulRemainedBytes),
                        (BOOL)pstGlbVar->stDatCmdInfo.ulWrite);

                pstGlbVar->stDatCmdInfo.ulRemainedBytes -= ulReadBytes;
            }
        }/* end of if(remainedbytes!=0) */

        pstGlbVar->stDatCmdInfo.ulCmdInProcess = 0;  /* Clear Data command in process flag. */
    }/*End of checking for Data done. */
}

/*****************************************************************************
* 函 数 名  : emmc_data_event_handler
*
* 功能描述  : 处理数据命令相关的事件
*
* 输入参数  : UINT32 ulIntStatus  :中断状态
*             UINT32 ulIdsStatus  :IDMAC状态
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
void emmc_data_event_handler(EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulIntStatus)
{
    UINT32  ulReadBytes = 0 ;

    /*
        For data transfer intrs,check data command is in process.
        Is this interrupt after ABORT/STOP command sent ?
    */
    if(!pstGlbVar->stDatCmdInfo.ulCmdInProcess)
    {
        return ;
    }

    emmc_data_event_error_handler(pstGlbVar,ulIntStatus);

    emmc_data_event_dto_handler(pstGlbVar,ulIntStatus);

    /*
        In any case,handle this as clock is stoped by IP and unless
        FIFO is cleared/filled,no command is further processed.
        Check if(Data starvation interrupt)
    */
    if(ulIntStatus & EMMC_INT_BIT_HTO) /* Data_starv_tmout */
    {
        /* If 0 bytes to be written,do nothing.Mistimed interrupt. */
        if(pstGlbVar->stDatCmdInfo.ulRemainedBytes)
        {
            ulReadBytes = emmc_read_write_fifo(pstGlbVar->stDatCmdInfo.ulRemainedBytes,
                    (pstGlbVar->stDatCmdInfo.pucCurrDataBuff + pstGlbVar->stDatCmdInfo.ulByteCount - pstGlbVar->stDatCmdInfo.ulRemainedBytes),
                     (BOOL)pstGlbVar->stDatCmdInfo.ulWrite);

            pstGlbVar->stDatCmdInfo.ulRemainedBytes -= ulReadBytes;
        }/* End of Data starvation check */
    }/* End of starvation interrupt check. */
}

/*****************************************************************************
* 函 数 名  : emmc_read_response
*
* 功能描述  : 读取命令回应
*
* 输入参数  : EMMC_RESPONSE_INFO_S * pstResponseInfo  :The response buffer
*             int ulRespType :0 means short response.others means long response
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
VOID emmc_read_response( EMMC_RESPONSE_INFO_S * pstResponseInfo, UINT32 ulRespType)
{
    pstResponseInfo->ulResp0 = INREG32(EMMC_REG_RESP0);

    if(ulRespType) /* Long response */
    {
        pstResponseInfo->ulResp1 = INREG32(EMMC_REG_RESP1);
        pstResponseInfo->ulResp2 = INREG32(EMMC_REG_RESP2);
        pstResponseInfo->ulResp3 = INREG32(EMMC_REG_RESP3);
    }
}

/*****************************************************************************
* 函 数 名  : emmc_event_Handler
*
* 功能描述  : 命令事件处理
*
* 输入参数  : UINT32 ulIntStatus  :interrupt status
*             UINT32 ulIdsStatus  :ids status
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
VOID emmc_event_Handler(EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulIntStatus, UINT32 ulIdsStatus)
{
    if(ulIdsStatus & (1<<2))    /* FBE */
    {
        print_info("\r\nfatal bus error.");
    }

    if(ulIdsStatus & (1<<4))    /* DU */
    {
        print_info("\r\ndescriptor unavailable.");
#ifdef EMMC_IDMAC_SUPPORT
        EMMC_IDMAC_RESUME_DMA();
#endif
    }

    /* If it is a data command */
    if(pstGlbVar->stDatCmdInfo.ulCmdInProcess)
    {
        emmc_data_event_handler(pstGlbVar, ulIntStatus);
    }

    /*
        Auto comand done (bit14) is received for data command only.
        If ABORT/STOP command is already sent,then do nothing for this.
    */
    if((ulIntStatus & EMMC_INT_BIT_ACD) && (pstGlbVar->stDatCmdInfo.ulCmdInProcess))
    {
        /*
            For data command,since stop is sent by IP,then donot send stop again.
            If ABORT_IN_PROCESS,then do nothing for this interrupt.
            Command done for abort command will be received soon.
        */
    }

    /*
        If any active command is not in process,then this is a false
        trigger.Donot respond to this.Response buffer pointers invalid.
        **** IMP : tCurrCmdInfo.fCurrCmdInprocess is set when command is loaded.
        Also,this will not respond to command_done received when
        clock parameters are changed.
    */
    if((ulIntStatus & EMMC_INT_BIT_CD) && (pstGlbVar->stCurrCmdInfo.ulCurrCmdInProcess))
    {
        /*
            sd_mmc_event: entering in command_done handling.
            Check if any other response error set.(bits 1,6,8)
        */
        if((ulIntStatus & EMMC_INT_BIT_RE)
            ||  (ulIntStatus & EMMC_INT_BIT_RCRC)
            ||  (ulIntStatus & EMMC_INT_BIT_RTO))
        {
            pstGlbVar->stCurrCmdInfo.ulCurrErrorSet = 1; /* set error flag */
            pstGlbVar->stCurrCmdInfo.ulCurrErrorSts |= (ulIntStatus & (EMMC_INT_BIT_RE|EMMC_INT_BIT_RCRC|EMMC_INT_BIT_RTO));
        }
        else
        {
            /*
                sd_mmc_event: Reading response.
                Copy response. Pass if long or short response.
                For long response all 4 resp.reg.sare copied and
                for short response,only resp.reg.0 is copied.
            */
            if(pstGlbVar->stCurrCmdInfo.pucCurrCmdRespBuff)
            {
                emmc_read_response((EMMC_RESPONSE_INFO_S *)(pstGlbVar->stCurrCmdInfo.pucCurrCmdRespBuff),
                               pstGlbVar->stCurrCmdInfo.ulResponsetype);/*lint !e826*/
            }
        }

        /*
            Set command over flag.
            This flag is polled by emmc_send_command_to_host().
        */
        pstGlbVar->stCurrCmdInfo.ulCurrCmdInProcess = 0;
    }/* If command done interrupt. */
    else
    {
        /* print_info( "!CD\r\n" ); */
    }
}

UINT32 emmc_unpack_csd( UINT8 *pucResp, CSD_REG_U *punCSD, BOOL bSDcard)
{
    UINT8 *buf = pucResp;

    punCSD->Fields.csd_structure = (buf[15] & 0xc0) >> 6;
    punCSD->Fields.spec_vers = (buf[15] & 0x3c) >> 2;
    punCSD->Fields.taac = buf[14];
    punCSD->Fields.nsac = buf[13];
    punCSD->Fields.tran_speed = buf[12];
    punCSD->Fields.ccc = (*((UINT16*)(&buf[10])) & 0xfff0) >> 4;/*lint !e826*/
    punCSD->Fields.read_bl_len = buf[10] & 0x0f;
    punCSD->Fields.read_bl_partial = (buf[9] & 0x80) ? 1 : 0;
    punCSD->Fields.write_blk_misalign = (buf[9] & 0x40) ? 1 : 0;
    punCSD->Fields.read_blk_misalign = (buf[9] & 0x20) ? 1 : 0;
    punCSD->Fields.dsr_imp = (buf[9] & 0x10) ? 1 : 0;
    if  (!bSDcard || !punCSD->Fields.csd_structure)//(!punCSD->Fields.csd_structure)
    {
        punCSD->Fields.c_size = ((*((UINT16*)(&buf[8])) & 0x3ff) << 2) |
                                ((UINT16)(buf[7] & 0xc0) >> 6);/*lint !e826*/

        punCSD->Fields.vdd_r_curr_min = (buf[7] & 0x38) >> 3;

        punCSD->Fields.vdd_r_curr_max = buf[7] & 0x07;
        punCSD->Fields.vdd_w_curr_min = (buf[6] & 0xe0) >> 5;
        punCSD->Fields.vdd_w_curr_max = (buf[6] & 0x1c) >> 2;
        punCSD->Fields.c_size_mult = ((buf[6] & 0x03) << 1) |
                                     ((buf[5] & 0x80) >> 7);
    }
    else
    {
        //punCSD->Fields.c_size = *((INT32 *)(&buf[6])) & 0x03fffff;
        punCSD->Fields.c_size = emmc_byte2ulong(&buf[6]);
    }

    /* bits 46-32 are different for SD and MMC cards. */
    if (bSDcard)    /* SD card */
    {
        punCSD->Fields.erase.v22.erase_grp_size = (buf[5] & 0x40) >> 6;
        punCSD->Fields.erase.v22.sector_size = ((buf[5] & 0x3f) << 1) |
                                               ((buf[4] & 0x80) >> 7);
        punCSD->Fields.wp_grp_size = buf[4] & 0x7f;
    }
    else /* MMC card */
    {
        switch (punCSD->Fields.csd_structure)
        {
            case CSD_STRUCT_VER_1_0:
            case CSD_STRUCT_VER_1_1:
                punCSD->Fields.erase.v22.sector_size = (buf[5] & 0x7c) >> 2;
                punCSD->Fields.erase.v22.erase_grp_size = ((buf[5] & 0x03) << 3) |
                                                          ((buf[4] & 0xe0) >> 5);
                break;
            case CSD_STRUCT_VER_1_2:
            default:
                punCSD->Fields.erase.v31.erase_grp_size = (buf[5] & 0x7c) >> 2;
                punCSD->Fields.erase.v31.erase_grp_mult = ((buf[5] & 0x03) << 3) |
                                                          ((buf[4] & 0xe0) >> 5);
                break;
        }

        punCSD->Fields.wp_grp_size = buf[4] & 0x1f;
    }

    punCSD->Fields.wp_grp_enable = (buf[3] & 0x80) ? 1 : 0;
    punCSD->Fields.default_ecc = (buf[3] & 0x60) >> 5;
    punCSD->Fields.r2w_factor   = (buf[3] & 0x1c) >> 2;
    punCSD->Fields.write_bl_len = ((buf[3] & 0x03) << 2) |
                                  ((buf[2] & 0xc0) >> 6);
    punCSD->Fields.write_bl_partial = (buf[2] & 0x20) ? 1 : 0;
    punCSD->Fields.file_format_grp = (buf[1] & 0x80) ? 1 : 0;
    punCSD->Fields.copy = (buf[1] & 0x40) ? 1 : 0;
    punCSD->Fields.perm_write_protect = (buf[1] & 0x20) ? 1 : 0;
    punCSD->Fields.tmp_write_protect = (buf[1] & 0x10) ? 1 : 0;
    punCSD->Fields.file_format = (buf[1] & 0x0c) >> 2;
    punCSD->Fields.ecc = buf[1] & 0x03;

    switch (punCSD->Fields.csd_structure)
    {
        case CSD_STRUCT_VER_1_0:
        case CSD_STRUCT_VER_1_1:
            /* DEBUG(2," V22 sector_size=%d erase_grp_size=%d\n",
            csd->erase.v22.sector_size,
            csd->erase.v22.erase_grp_size); */
            break;
        case CSD_STRUCT_VER_1_2:
        default:
            /* DEBUG(2," V31 erase_grp_size=%d erase_grp_mult=%d\n",
            csd->erase.v31.erase_grp_size,
            csd->erase.v31.erase_grp_mult);*/
            break;
    }

    return ERRNOERROR;
}

UINT32 emmc_unpack_cid( UINT8 *pucResp, CID_REG_U *pstCid)
{
    memcpy(pstCid->cCid, pucResp, sizeof(pstCid->cCid));

    return ERRNOERROR;
}

/*****************************************************************************
* 函 数 名  : mmc_unpack_r1
*
* 功能描述  : from card status(get from r1),check for possible error bits.
*
* 输入参数  : UINT8 * pucResp:Response buffer.
*             enum card_state state:Enumerated type holding state of card,
*                                   before execution of command.
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  : Called by validate_response(),as per command index,response
*             type is decided.
*
*****************************************************************************/
UINT32 mmc_unpack_r1(UINT8 *pucResp, enum card_state state)
{
    UINT32 ulCardStatus = 0x0;

    ulCardStatus = *(UINT32 *)pucResp;/*lint !e826*/

    /* From card status,check for possible error bits. */
    if (R1_STATUS(ulCardStatus))
    {
        if (ulCardStatus & R1_OUT_OF_RANGE)
        {
            return MMC_ERROR_OUT_OF_RANGE;
        }

        if (ulCardStatus & R1_ADDRESS_ERROR)
        {
            return MMC_ERROR_ADDRESS;
        }

        if (ulCardStatus & R1_BLOCK_LEN_ERROR)
        {
            return MMC_ERROR_BLOCK_LEN;
        }

        if (ulCardStatus & R1_ERASE_SEQ_ERROR)
        {
            return MMC_ERROR_ERASE_SEQ;
        }

        if (ulCardStatus & R1_ERASE_PARAM)
        {
            return MMC_ERROR_ERASE_PARAM;
        }

        if (ulCardStatus & R1_WP_VIOLATION)
        {
            return MMC_ERROR_WP_VIOLATION;
        }

        /* Status bit. No need to check. Error reflected in Lock_Unlock_failed. */
        /* if ( ulCardStatus & R1_CARD_IS_LOCKED )     return MMC_ERROR_CARD_IS_LOCKED; */
        if (ulCardStatus & R1_LOCK_UNLOCK_FAILED)
        {
            return MMC_ERROR_LOCK_UNLOCK_FAILED;
        }

        /* Error of previous command.ulf required user will read using CMD13.
           if ( ulCardStatus & R1_COM_CRC_ERROR )      return MMC_ERROR_COM_CRC;
           if ( ulCardStatus & R1_ILLEGAL_COMMAND )    return MMC_ERROR_ILLEGAL_COMMAND;
         */

        if (ulCardStatus & R1_CARD_ECC_FAILED)
        {
            return MMC_ERROR_CARD_ECC_FAILED;
        }

        if (ulCardStatus & R1_CC_ERROR)
        {
            return MMC_ERROR_CC;
        }

        if (ulCardStatus & R1_ERROR)
        {
            return MMC_ERROR_GENERAL;
        }

        if (ulCardStatus & R1_UNDERRUN)
        {
            return MMC_ERROR_UNDERRUN;
        }

        if (ulCardStatus & R1_OVERRUN)
        {
            return MMC_ERROR_OVERRUN;
        }

        if (ulCardStatus & R1_CID_CSD_OVERWRITE)
        {
            return MMC_ERROR_CID_CSD_OVERWRITE;
        }

        /* Status bits.To be checked for erase commands. */
    }

    /* Check if card state when command is received matches
       with what should be.This can be used to confirm that no state is
       changed. */
    if (CARD_STATE_BOTTOM != state)
    {
        /* Donot match states,if state code is 0x0ff. */
        if ((enum card_state)R1_CURRENT_STATE(ulCardStatus) != state)
        {
            return ERRFSMSTATE;
        }
    }

    return ERRNOERROR;
}

UINT32  emmc_validate_response(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulCmdId, UINT8 *pucResp, UINT32 ulCmpState)
{
    UINT32 ulRetVal = 0;

    switch (ulCmdId)
    {
        case CMD1: /* SEND_OP_COND */
            pstGlbVar->stCardInfo[slot].card_ocr = *(UINT32 *)pucResp; /*lint !e826*/
            return ERRNOERROR;

        case CMD9: /* SEND_CSD */
            return emmc_unpack_csd(pucResp, &(pstGlbVar->stCardInfo[slot].CSD),
                ((CARD_TYPE_MMC!=pstGlbVar->stCardInfo[slot].card_type)
                &&(CARD_TYPE_MMC_42!=pstGlbVar->stCardInfo[slot].card_type)));

        /* During CMD2,response holds the CID data of the card which
           wins in sending CIDs on command line.
           After enumeration,CID data can be obtained through CMD10.
        */
        case CMD2:  /* ALL_SEND_CID */
        case CMD10: /* SEND_CID */
            ulRetVal = emmc_unpack_cid(pucResp,&pstGlbVar->stCardInfo[slot].CID);
            return ulRetVal;

        case CMD39: /* FAST_IO R4 response. */
            return ERRNOERROR;

        case CMD40: /* GO_IRQ_STATE R5 response. */
            return ERRNOERROR;

        default: /* R1,R1b Response. */

            /* Checks if there is any error in response.
               Second parameter is expected state of card prior
               to execution of the command.To by-pass this check
               pass this value as 0xff. */
            ulRetVal = mmc_unpack_r1(pucResp, (enum card_state)ulCmpState);
            return ulRetVal;
    }
}

VOID emmc_config_od(BOOL bEnable)
{
    if(bEnable)
    {
        SETREG32(EMMC_REG_CTRL,1<<24);
    }
    else
    {
        CLRREG32(EMMC_REG_CTRL,1<<24);
    }
}

UINT32 emmc_reset_ip(VOID)
{
    volatile UINT32  ulData = 0;
    TIMER0_VAR_DECLAR;

    /*
        Reset IP & FIFO & CONTROLLER
    */
    ulData  = INREG32(EMMC_REG_CTRL);
    ulData |= 0x7;
    OUTREG32(EMMC_REG_CTRL, ulData);

    TIMER0_INIT_TIMEOUT(EMMC_RESET_IP_TIMEOUT);
    for( ; ; )
    {
        ulData = INREG32(EMMC_REG_CTRL);
        if(0 == (ulData & 0x7))   /* The fifo/ip reset finish  */
        {
            break;
        }

        if(TIMER0_IS_TIMEOUT())
        {
            return ERRHARDWARE;
        }
    }

    return ERRNOERROR;
}

UINT32 emmc_reset_fifo(VOID)
{
    UINT32 ulData = 0;
    TIMER0_VAR_DECLAR;

    SETREG32(EMMC_REG_CTRL, EMMC_CTRL_BIT_FIFO_RESET);

    TIMER0_INIT_TIMEOUT( EMMC_RESET_FIFO_TIMEOUT);
    /* Wait till Fifo Reset bit gets cleared. */
    for(;;)
    {
        ulData = INREG32(EMMC_REG_CTRL);
        if(TIMER0_IS_TIMEOUT())
        {
            return ERRHARDWARE;
        }

        if(!(ulData & EMMC_CTRL_BIT_FIFO_RESET))
        {
            break;
        }
    }

    return ERRNOERROR;
}

VOID emmc_interrupt_setup(VOID)
{
    OUTREG32( EMMC_REG_RINTSTS, 0xFFFFFFFF);
    OUTREG32( EMMC_REG_INTMASK, 0x00000000);
    CLRREG32(EMMC_REG_CTRL, EMMC_CTRL_BIT_INTENABLE);
}

VOID emmc_global_variable_init(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, BOOL bUseHold)
{
    memset((VOID *)pstGlbVar,0,sizeof(EMMC_GLOBAL_VAR_S));
    pstGlbVar->slot = slot;
    pstGlbVar->stIPStatusInfo.bUseHold = bUseHold;
}

#ifdef EMMC_DEBUG
VOID emmc_cardinfo_show(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    EMMC_CARD_INFO_S *pstCardInfo = &(pstGlbVar->stCardInfo[slot]);

    print_info_with_u32("\r\ncard_type          :0x",pstCardInfo->card_type);
    print_info_with_u32("\r\ncard_size          :0x",(UINT32)pstCardInfo->card_size);/*lint !e747*/
    print_info_with_u32("\r\ncard_rca           :0x",pstCardInfo->card_rca);
    print_info_with_u32("\r\ncard_ocr           :0x",pstCardInfo->card_ocr);
    print_info_with_u32("\r\ncard_read_blksize  :0x",pstCardInfo->card_read_blksize);
    print_info_with_u32("\r\ncard_write_blksize :0x",pstCardInfo->card_write_blksize);
    print_info_with_u32("\r\nextcsd_bootop      :0x",(UINT32)pstCardInfo->extcsd_bootop);
    print_info_with_u32("\r\nextcsd_seccount    :0x",pstCardInfo->extcsd_seccount);
    print_info_with_u32("\r\nextcsd_bootinfo    :0x",pstCardInfo->extcsd_bootinfo);
    print_info_with_u32("\r\nextcsd_bootsizemult:0x",pstCardInfo->extcsd_bootsizemult);
    print_info_with_u32("\r\nextcsd_partitioncfg:0x",pstCardInfo->extcsd_partitioncfg);
    print_info_with_u32("\r\nextcsd_bootbuswidth:0x",pstCardInfo->extcsd_bootbuswidth);
}
#endif

UINT32  emmc_poll_cmd_register(VOID)
{
    TIMER0_VAR_DECLAR;

    /* total time is 100ms */
    TIMER0_INIT_TIMEOUT(EMMC_CMD_ACCEPT_TIMEOUT);
    for( ; ; )
    {
        /*
            Check if CMD::start_cmd bit is clear. start_cmd = 0 means BIU
            has loaded registers and next command can be loaded in.
        */
        if(0 == (INREG32(EMMC_REG_CMD) & EMMC_CMD_BIT_START))
        {
            return ERRNOERROR;
        }

        /* Check if HLE bit is set. */
        if(INREG32( EMMC_REG_RINTSTS ) & EMMC_INT_BIT_HLE)
        {
            return ERRHLE;
        }

        if(TIMER0_IS_TIMEOUT())
        {
            /* Timeout, the IP/emmc not accept the command */
            return ERRHARDWARE;
        }
    }
}

/**
  * This function does the final part of sending commands.
  * It sets the CMD register and CMD ARG register
  */
UINT32 emmc_execute_command(EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 cmd_register, UINT32 arg_register)
{
    COMMAND_INFO_S stCmdInfo;

    memset((VOID *)&stCmdInfo,0,sizeof(COMMAND_INFO_S));
    stCmdInfo.CmdReg.reg = cmd_register;
    stCmdInfo.CmdArg = arg_register;

    return emmc_send_command(pstGlbVar, &stCmdInfo);
}

/**
  * Sends the clock only command.
  * This function loads the clock settings to the card. Does not pass
  * an messages to the card.
  *
  * \return 0 upon success. Error status upon failure.
  */
UINT32 emmc_send_clock_only_cmd(EMMC_GLOBAL_VAR_S *pstGlbVar)
{
	return emmc_execute_command(pstGlbVar, EMMC_CMD_BIT_START|EMMC_CMD_BIT_UPDATE_CLK_ONLY | EMMC_CMD_BIT_WAIT_PRV_COMPLETE, 0);
}

/**
  * Enables clocks to a card slot in the controller.
  */
UINT32 emmc_enable_clock(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    SETREG32(EMMC_REG_CLKENA, 1<<slot);
	return emmc_send_clock_only_cmd(pstGlbVar);
}

/**
  * Disable clocks to a card slot in the controller.
  */
UINT32 emmc_disable_clock(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    CLRREG32(EMMC_REG_CLKENA, (UINT32)(1<<slot));
	return emmc_send_clock_only_cmd(pstGlbVar);
}

/**
  * Disables all clocks to the controller.
  */
UINT32 emmc_disable_all_clocks(EMMC_GLOBAL_VAR_S *pstGlbVar)
{
	OUTREG32(EMMC_REG_CLKENA, 0);
	return emmc_send_clock_only_cmd(pstGlbVar);
}

UINT32 emmc_enable_clocks_with_val(EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 val)
{
	OUTREG32(EMMC_REG_CLKENA, val);
	return emmc_send_clock_only_cmd(pstGlbVar);
}

/**
  * Sets the divider for the clock in CIU.
  * This function sets a particular divider to the clock.
  * @param[in] divider The divider value.
  * \return 0 upon success. Error code upon failure.
  */
UINT32 emmc_set_clk_freq(UINT32 slot,EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 clksrc,UINT32 divider)
{
	UINT32 orig_clkena = 0;
	UINT32 retval = 0;

	if (divider > MAX_DIVIDER_VALUE)
    {
    	return 0xffffffff;
    }

    /* To make sure we don't disturb enable/disable settings of the cards*/
	orig_clkena = INREG32(EMMC_REG_CLKENA);

    /* Disable specific slotc clock before changing frequency the of card clocks */
	if ((retval = emmc_disable_clock(slot,pstGlbVar)))
    {
    	return retval;
    }

    /* Program the clock divider in our case it is divider 0 */
	SETBITVALUE32(EMMC_REG_CLKDIV, (UINT32)(0xff<<(clksrc*8)), divider);

    /* Send the command to CIU using emmc_send_clock_only_cmd and enable the clocks in CLKENA register */
	if ((retval = emmc_send_clock_only_cmd(pstGlbVar))) {
        (VOID)emmc_enable_clocks_with_val(pstGlbVar,orig_clkena);
    	return retval;
    }

	return emmc_enable_clocks_with_val(pstGlbVar,orig_clkena);
}

UINT32 emmc_set_default_clk(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal;
    UINT32 ulClkDivdata;

    if ((CARD_TYPE_MMC == pstGlbVar->stCardInfo[slot].card_type)
        ||(CARD_TYPE_MMC_42 == pstGlbVar->stCardInfo[slot].card_type))
    {
        ulClkDivdata = MMC_FREQDIVIDER;
    }   /* divide by 2. */
    else
    {
        ulClkDivdata = SD_FREQDIVIDER;
    }

    ulRetVal = emmc_set_clk_freq(slot, pstGlbVar, EMMC_CURRENT_CLKSRC,ulClkDivdata);

    return ulRetVal;
}

UINT32 emmc_get_fifodepth(VOID)
{
    UINT32 u32FifoDepth = 0;

    /* Get FIFOTH */
    if(0 == EMMC_GET_TX_WMARK(INREG32(EMMC_REG_FIFOTH)))    /* power on default */
    {
        u32FifoDepth = EMMC_GET_RX_WMARK(INREG32(EMMC_REG_FIFOTH))+1;
    }
    else    /* has been configed */
    {
        u32FifoDepth = EMMC_GET_TX_WMARK(INREG32(EMMC_REG_FIFOTH))*2;
    }


    return u32FifoDepth;
}

VOID emmc_set_fifothresh(VOID)
{
    UINT32 u32FifoDepth = emmc_get_fifodepth();

    /* Tx Watermark */
    SETBITVALUE32(EMMC_REG_FIFOTH, 0xfff, (u32FifoDepth/2));

    /* Rx Watermark */
    SETBITVALUE32(EMMC_REG_FIFOTH, 0xfff0000, ((u32FifoDepth/2-1)<<16));

    SETBITVALUE32(EMMC_REG_FIFOTH, 0x70000000,FIFOTH_MSIZE_128);
}

/*****************************************************************************
* 函 数 名  : emmc_read_write_fifo
*
* 功能描述  : read/write data to/from fifo.
*
* 输入参数  : UINT32 ulByteCount  :byte count
*             UINT8 *pucDataBuff  :data buffer
*             BOOL bWrite         :write/read
* 输出参数  : 无
*
* 返 回 值  : 实际读取长度
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_read_write_fifo(UINT32 ulByteCount, UINT8 *pucDataBuff, BOOL bWrite)
{
    UINT32 ulStatusData, ulValue, ulRetVal;
    UINT32 ulDatalength = 0;
    UINT32 ulDataBytes, i;
    UINT32 u32FifoDepth = emmc_get_fifodepth();

    if (0 == ulByteCount)
    {
        return 0;
    }

    ulStatusData = INREG32(EMMC_REG_STATUS);
    if (bWrite) /* write to fifo. */
    {
        if(ulStatusData & 0x8)      /* Check if fifo is full. */
        {
            return 0;   /* No bytes written. */
        }
        else
        {
            ulDatalength = (ulStatusData & 0x4)?u32FifoDepth:(u32FifoDepth - EMMC_GET_FIFO_COUNT(ulStatusData));
        }

        /* Check if bytes to write are more than ulDatalength. */
        if (ulDatalength > (ulByteCount / (EMMC_FIFO_WIDTH/8)))
        {
            ulDatalength = (ulByteCount / (EMMC_FIFO_WIDTH/8));
        }

        /* If byte count is < FIFO width. */
        if(0 == ulDatalength)
        {
            ulDatalength = 1;
            ulDataBytes = ulByteCount;
        }
        else
        {
            ulDataBytes = (EMMC_FIFO_WIDTH / 8);
        }

        /* ulDatalength is in terms of FIFO size units. */
        for (i = 0; i < ulDatalength; i++)
        {
            memcpy((VOID *)&ulValue, (pucDataBuff + (i * ulDataBytes)), ulDataBytes);

            /* Write FIFO. */
            OUTREG32(EMMC_REG_FIFO, ulValue);
        }
    }
    else /* read from fifo. */
    {
        /* Check if fifo is not empty. */
        if (ulStatusData & 0x4)
        {
            return 0;   /* No bytes read. */
        }
        else
        {
            ulDatalength = (ulStatusData & 0x8)?u32FifoDepth:EMMC_GET_FIFO_COUNT(ulStatusData);
        }

        /* Check if bytes to read are less than ulDatalength. */
        if (ulDatalength > ((ulByteCount) / (EMMC_FIFO_WIDTH / 8)))
        {
            /* FIFO width is in bits. */
            ulDatalength = ((ulByteCount )/ (EMMC_FIFO_WIDTH / 8));
        }

        /* If byte count is < FIFO width. */
        if (0 == ulDatalength)
        {
            ulDatalength = 1;
            ulDataBytes = ulByteCount;
        }
        else
        {
            ulDataBytes = ((EMMC_FIFO_WIDTH) / 8);
        }

        /* ulDatalength is in terms of FIFO size units. */
        for (i = 0; i < ulDatalength; i++)
        {
            ulValue = INREG32(EMMC_REG_FIFO);

            memcpy((VOID *)(pucDataBuff + (i * ulDataBytes)), (const VOID *)&ulValue, ulDataBytes);
        }
    }

    ulRetVal = (i * ulDataBytes);

    return ulRetVal;
}/*lint !e715*/

/*****************************************************************************
* 函 数 名  : emmc_send_command
*
* 功能描述  : 加载命令
*
* 输入参数  : COMMAND_INFO_S *pstCmdInfo    :命令结构体
* 输出参数  : 无
*
* 返 回 值  : ERRNOERROR :命令加载成功
*             ERRHLE     :HLE
*             ERRHARDWARE:硬件错误
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32  emmc_send_command(EMMC_GLOBAL_VAR_S *pstGlbVar, COMMAND_INFO_S *pstCmdInfo)
{
    /* Write byte count register */
    OUTREG32(EMMC_REG_BYTCNT, pstCmdInfo->ByteCnt);

    /* Write block size register. */
    OUTREG32(EMMC_REG_BLKSIZ, pstCmdInfo->BlkSize);

    OUTREG32(EMMC_REG_CMDARG, pstCmdInfo->CmdArg);

    pstCmdInfo->CmdReg.Bits.use_hold_reg = pstGlbVar->stIPStatusInfo.bUseHold?1:0;

    /* Write command register. */
    OUTREG32(EMMC_REG_CMD, pstCmdInfo->CmdReg.reg);

#ifdef EMMC_DEBUG
    print_info_with_u32("\r\nCMD    :0x",pstCmdInfo->CmdReg.reg);
    print_info_with_u32("\r\nCMDARG :0x",pstCmdInfo->CmdArg);
    print_info_with_u32("\r\nByteCnt:0x",pstCmdInfo->ByteCnt);
    print_info_with_u32("\r\nBlkSize:0x",pstCmdInfo->BlkSize);
#endif

    return emmc_poll_cmd_register();
}/*lint !e550*/


/*****************************************************************************
* 函 数 名  : emmc_wait_cmd_complete
*
* 功能描述  : 等待命令完成
*
* 输入参数  : UINT32 ulDataCmd  :数据命令
*             UINT32 ulWrite    :读/写标志
* 输出参数  : 无
*
* 返 回 值  : ERRNOERROR        :成功
*             ERRRETRYTIMEOUT   :重试超时
*             其它              :失败
*
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_wait_cmd_complete(EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulDataCmd, UINT32 ulWrite)
{
    TIMER0_VAR_DECLAR;
    UINT32 ulIntSts = 0;
    UINT32 ulIdsSts = 0;
    UINT32 ulRetVal = 0;
//    UINT32 ulTmpVal = 0;

    /* Disable interrupts and poll manually for events.  */
    TIMER0_INIT_TIMEOUT(EMMC_CMD_COMPLETE_TIMEOUT);
    for( ; ; )
    {
        /* read and clear interrupt. */
        ulIntSts = INREG32( EMMC_REG_RINTSTS);
        OUTREG32(EMMC_REG_RINTSTS, ulIntSts);
#ifdef EMMC_IDMAC_SUPPORT
        ulIdsSts = INREG32( EMMC_REG_IDSTS);
        OUTREG32(EMMC_REG_IDSTS, ulIdsSts);
#else
        ulIdsSts = 0;
#endif

/*
        ulTmpVal = pstGlbVar->stCurrCmdInfo.ulCurrErrorSts;
        print_info_with_u32("\r\nCmdErr :0x",ulTmpVal);
        ulTmpVal = pstGlbVar->stDatCmdInfo.ulDataErrStatus;
        print_info_with_u32("\r\DataErr :0x",ulTmpVal);
*/
        if(ulDataCmd)
        {
#ifdef EMMC_DEBUG
            print_info_with_u32("\r\n---->ulCurrErrorSts  =0x",pstGlbVar->stCurrCmdInfo.ulCurrErrorSts);
            print_info_with_u32("\r\n---->ulDataErrStatus =0x",pstGlbVar->stDatCmdInfo.ulDataErrStatus);
            print_info_with_u32("\r\n---->RINTSTS         =0x",ulIntSts);
            print_info_with_u32("\r\n---->IDSTS           =0x",ulIdsSts);
#endif
        }

        pstGlbVar->stCurrCmdInfo.ulCurrErrorSts |= (ulIntSts & 0x00002142);
#ifdef EMMC_IDMAC_SUPPORT
        pstGlbVar->stCurrCmdInfo.ulCurrErrorSts |= (ulIdsSts & 0x1C04)<<16;
#endif

        /* Check if any interrupt bit is set. then handle it */
        if((ulIntSts & 0xffff)||(ulIdsSts & 0x337))
        {
            emmc_event_Handler(pstGlbVar,ulIntSts,ulIdsSts);
        }
        else
        {
            if(TIMER0_IS_TIMEOUT())
            {
                //emmc_reg_dump();
                pstGlbVar->stDatCmdInfo.ulCmdInProcess      = 0;
                pstGlbVar->stCurrCmdInfo.ulCurrCmdInProcess = 0;
                return ERRRETRYTIMEOUT;
            }
        }

        /* If response timeout,data command is not sent. */
        if(ulIntSts & 0x0000100)
        {
            pstGlbVar->stDatCmdInfo.ulCmdInProcess = 0;
        }

        /*  If command done not received,check for it first. */
        if(0 == pstGlbVar->stCurrCmdInfo.ulCurrCmdInProcess)
        {
            if(ulDataCmd)
            {
                if(pstGlbVar->stDatCmdInfo.ulCmdInProcess)
                {
                    /* No data done. Continuing with loop */
                    continue;
                }
                else
                {
                    if(ulWrite)
                    {
                        /* Wait Card from busy */
                        if(emmc_wait_data0_ready())
                        {
                            return ERRRETRYTIMEOUT;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }

    /* Clear interrupt status register.Writing 1 clears the interrupt. */
    ulIntSts = INREG32(EMMC_REG_RINTSTS);
    OUTREG32(EMMC_REG_RINTSTS, ulIntSts);

    ulRetVal = ulDataCmd?(pstGlbVar->stCurrCmdInfo.ulCurrErrorSts|pstGlbVar->stDatCmdInfo.ulDataErrStatus):pstGlbVar->stCurrCmdInfo.ulCurrErrorSts;
    if(ulRetVal)
    {
#ifdef EMMC_DEBUG
        print_info_with_u32("\r\n---->ulCurrErrorSts  =0x",pstGlbVar->stCurrCmdInfo.ulCurrErrorSts);
        print_info_with_u32("\r\n---->ulDataErrStatus =0x",pstGlbVar->stDatCmdInfo.ulDataErrStatus);
#endif
    }
    return ulRetVal;
}

UINT32  emmc_send_cmd_to_host(EMMC_GLOBAL_VAR_S *pstGlbVar,
                          COMMAND_INFO_S *pstCmdInfo,
                          UINT8 *pucCmdRespBuff,
                          UINT8 *pucDataBuff,
                          UINT32 ulFlags,
                          UINT32 ulDataCmd,
                          UINT32 ulWrite) /* 0=User addr.space.buffer.*/
{
    UINT32 ulBytesWriten = 0, ulData = 0;
    CONTROL_REG_U ctrl_val;
    UINT32 ulRetVal = 0;
#ifdef EMMC_IDMAC_SUPPORT
    UINT32 ulNodeCnt = 0;
#endif

    /* 如果正在处理数据传输，不允许发送命令(STOP/ABORT除外). */
    if (pstGlbVar->stDatCmdInfo.ulCmdInProcess && (!(pstCmdInfo->CmdReg.reg & 0x4000)))
    {
        //print_info_with_u32("data cmd in process:0x", pstCmdInfo->CmdReg.reg);
        return ERRCMDINPROGRESS;
    }

    /* 初始化命令结构体. */
    pstGlbVar->stCurrCmdInfo.pucCurrDataBuff = pucDataBuff;
    pstGlbVar->stCurrCmdInfo.pucCurrCmdRespBuff = pucCmdRespBuff;
    pstGlbVar->stCurrCmdInfo.ulCurrCmdOptions = ulFlags; /* Option to retry/error check from user. */
    pstGlbVar->stCurrCmdInfo.ulCurrErrorSts   = 0x0;
    pstGlbVar->stCurrCmdInfo.ulCurrCmdInProcess = 1;
    pstGlbVar->stCurrCmdInfo.ulCurrErrorSet   = 0x0;
    pstGlbVar->stCurrCmdInfo.ulResponsetype = pstCmdInfo->CmdReg.Bits.response_length;

    if (ulDataCmd) /* 数据传输 */
    {
#ifdef EMMC_IDMAC_SUPPORT
        /* 计算本次传输需要的描述符个数 */
        ulNodeCnt = EMMC_IDMAC_GET_DESC_COUNT(pstCmdInfo->ByteCnt);
#endif

        /* 数据传输前，复位FIFO */
        (VOID)emmc_reset_fifo();

        ctrl_val.reg = INREG32(EMMC_REG_CTRL);

        /* 对于数据写命令，如果不使用DMA，将数据写入FIFO，返回值为实际写入到FIFO中的数据 */
        if (ulWrite && (!ctrl_val.Bits.use_internal_dmac))
        {
#ifndef EMMC_IDMAC_SUPPORT
            ulBytesWriten = emmc_read_write_fifo((UINT8*)pucDataBuff,
                                             pstCmdInfo->ByteCnt,
                                             ulWrite);
#endif
        }

        /* 对于数据传输命令，填充data command info结构，当出现错误时，需要对它进行清零 */
        pstGlbVar->stDatCmdInfo.pucCurrDataBuff = pucDataBuff;
        pstGlbVar->stDatCmdInfo.ulWrite = ulWrite;
        pstGlbVar->stDatCmdInfo.ulByteCount = pstCmdInfo->ByteCnt;
        pstGlbVar->stDatCmdInfo.ulRemainedBytes =
            ulWrite?(pstCmdInfo->ByteCnt - ulBytesWriten):pstCmdInfo->ByteCnt;
        pstGlbVar->stDatCmdInfo.ulCurrCmdOptions = ulFlags;
        pstGlbVar->stDatCmdInfo.ulCmdInProcess = 1; /* data command is in process. */
        pstGlbVar->stDatCmdInfo.ulDataErrStatus = 0x0;

#ifdef EMMC_IDMAC_SUPPORT
        if(ctrl_val.Bits.use_internal_dmac)    /* DMA传输数据 */
        {
            pstGlbVar->stDatCmdInfo.ulRemainedBytes = 0;

            /* 清除所有的中断状态 */
            EMMC_DMA_INTERRUPT_CLEAR(DmaStatusAbnormal|DmaStatusNormal|DmaStatusCardErrSum|DmaStatusNoDesc|
DmaStatusBusError|DmaStatusRxCompleted|DmaStatusTxCompleted);

            if(ulWrite)
            {
                emmc_idmac_nocopysend(&pstGlbVar->stIdmacHandle, (UINT32)pucDataBuff, (UINT32)pstCmdInfo->ByteCnt, ulNodeCnt);
            }
            else
            {
                emmc_idmac_nocopyreceive(&pstGlbVar->stIdmacHandle, (UINT32)pucDataBuff, (UINT32)pstCmdInfo->ByteCnt, ulNodeCnt);
            }
        }
#endif
    }
    else
    {
        pstGlbVar->stDatCmdInfo.ulDataErrStatus = 0x0;
    }

    /* 加载命令前，清除RINTSTS状态(除card_detect，否则将丢失插拔卡信息) */
    ulData = INREG32(EMMC_REG_RINTSTS);
    ulData &= 0xfffe;
    OUTREG32(EMMC_REG_RINTSTS, ulData);

    /* 加载命令 */
    ulRetVal = emmc_send_command(pstGlbVar, pstCmdInfo);

    /* Error likely is HLE error or internal error of OS. */
    if (0 != ulRetVal)
    {
        ulRetVal = ERRCMDNOTSUPP;
        return ulRetVal;
    }

    ulRetVal = emmc_wait_cmd_complete(pstGlbVar, ulDataCmd, ulWrite);
    if(ulRetVal)
    {
#ifdef EMMC_DEBUG
        print_info_with_u32("\r\nwait_cmd_complete res:0x",ulRetVal);
#endif
    }

    return ulRetVal;
}

/*****************************************************************************
* 函 数 名  : emmc_form_send_cmd
*
* 功能描述  : Data transfer commands are not handled here.Forms the command,
              and calls Send_Cmd_to_Host() which actually transfers command
              to host.ult will return  control when the command is executed
              by IP ie command done.
              Response will be copied in response buffer.
              -No error response buffer: As no ABORT/STOP for error in case
               of non data commands.
              -No need of data buffer :As this routine doesnot serve data
               commands.
              -No retries and no flags :For non data commands,no retries or
               automatic handling by driver.
*
* 输入参数  : INT32 slot
*             INT32 cmd_id
*             UINT32 UINT32CmdArg
*             UINT8 *pucCmdRespBuff
*             INT32 ulFlags
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_form_send_cmd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 cmd_id, UINT32 cmd_arg,
                       UINT8 *pucCmdRespBuff, UINT32 flags)
{
    UINT32 retval;
    COMMAND_INFO_S stCmdInfo;

	memset(&stCmdInfo,0,sizeof(stCmdInfo));

    if (slot >= EMMC_CARD_NUM)
    {
        return ERRPARAM;
    }

    /* Check if card is connected.
       For enumeration commands skip this check.
    */
    switch (cmd_id)
    {
        case SDIO_RESET:
        case CMD0:
        case CMD1:
        case CMD2:
        case CMD3:
        case CMD5:
        case CMD8:
        case CMD55:
        case ACMD41:
            break;

        default:
            if(!emmc_get_card_present(slot))
            {
                return ERRCARDNOTCONN;
            }
    }

    /* Form command */
    switch (cmd_id)
    {
        /* 1. No response commands. */
        case CMD0: //Argument is stuff bits.
        case CMD4: //DSR Argument as supplied by user.
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 0; //Short resp.
            stCmdInfo.CmdReg.Bits.response_expect = 0; //No response
            stCmdInfo.CmdArg = cmd_arg;
            break;

        case CMD8:
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 0; //R7 response
            stCmdInfo.CmdReg.Bits.response_expect = 1;
            stCmdInfo.CmdArg = cmd_arg;
            break;

        /* 2. Long response. */
        case CMD2:
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 1; //R2 response
            stCmdInfo.CmdReg.Bits.response_expect = 1;
            break;

        /* 3. Short response. */
        case SDIO_RESET:
        case CMD52:     //SDIO RW DIRECT.Argument is command block
        case SDIO_ABORT:
        case CMD38:     //Argument is stuff bits.
        case CMD40:     //Argument is stuff bits.
        case CMD1:      //OCR Argument as supplied by user.
        case CMD3:      //Argument is RCA for MMC card.
        case CMD16:     //Block length.
        case CMD23:     //Number of block counts.
        case CMD28:     //Set write protect
        case CMD29:     //clear write protect.
        case CMD32:     //SD erase Data start address.
        case CMD33:     //SD erase Data end address.
        case CMD35:     //Data address.
        case CMD36:     //Data address.
        case CMD56:     //Bit0 = RD/WR.
        case ACMD41:    //Argument is OCR without busy.
        case CMD6:      //HSMMC:SWITCH command. For ACMD6:Arg.uls Bus Width.
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 0; //Short resp.
            stCmdInfo.CmdReg.Bits.response_expect = 1;
            stCmdInfo.CmdArg = cmd_arg;

            if (cmd_id == SDIO_ABORT)
            {
                stCmdInfo.CmdReg.Bits.stop_abort_cmd = 1;
            }
            break;

        /* 4. STOP/ABORT command. */
        case CMD12: //Stop command.
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 1; //STOP command.
            stCmdInfo.CmdReg.Bits.response_length = 0; //Short resp.
            stCmdInfo.CmdReg.Bits.response_expect = 1;

            break;

        /* 5. Argument is RCA.Found from thepstGlbVar->stCardInfo structure. */
        case CMD7:
        case CMD13:
        case CMD55:
            //Copy RCA from card info structure into
            //argument.RCA is from 16-31 bits.
            stCmdInfo.CmdArg = pstGlbVar->stCardInfo[slot].card_rca<<16;
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 0; //Short resp.
            stCmdInfo.CmdReg.Bits.response_expect = 1;

            break;

        case CMD15: //No response
            //Copy RCA from card info structure into
            //argument.RCA is from 16-31 bits.
            stCmdInfo.CmdArg = pstGlbVar->stCardInfo[slot].card_rca<<16;
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 0;
            stCmdInfo.CmdReg.Bits.response_expect = 0; //No response
            break;

        case CMD9: //Long response
        case CMD10:
            //Copy RCA from card info structure into
            //argument.RCA is from 16-31 bits.
            stCmdInfo.CmdArg = pstGlbVar->stCardInfo[slot].card_rca<<16;
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 1; //Long response
            stCmdInfo.CmdReg.Bits.response_expect = 1;
            break;

        case CMD5: //IO_SEND_OP_COND command.
            stCmdInfo.CmdReg.Bits.stop_abort_cmd  = 0;
            stCmdInfo.CmdReg.Bits.response_length = 0; //Short resp.
            stCmdInfo.CmdReg.Bits.response_expect = 1; //No response
            stCmdInfo.CmdArg = cmd_arg;
            break;

        default:
            print_info_with_u32("form_send_cmd:cmd id(%#x) not matched.\n", cmd_id);
            return ERRCMDNOTSUPP;
    } //End of switch statement.

    /* Default parameters. */
    stCmdInfo.CmdReg.Bits.start_cmd = 1;
    stCmdInfo.CmdReg.Bits.Update_clk_regs_only = 0; //Normal command.

    if ((CMD0 == cmd_id) || (SDIO_RESET == cmd_id))
    {
        /* Send initialisation sequence for reset commands.
           These commands are after power ON.So,ramp up time
           requd.
         */
        stCmdInfo.CmdReg.Bits.send_initialization = 1;
    }//init.
    else
    {
        stCmdInfo.CmdReg.Bits.send_initialization = 0;
    }//No init.

    stCmdInfo.ByteCnt = 0x0; //Only for data command.
    stCmdInfo.BlkSize = 0x0; //Only for data command.
    stCmdInfo.CmdReg.Bits.transfer_mode = 0;    //Only for data command.
    stCmdInfo.CmdReg.Bits.read_write = 0x0;     //Only for data command.
    stCmdInfo.CmdReg.Bits.data_expected = 0;    //No data command.
    if (cmd_id == CMD52)
    {
        stCmdInfo.CmdReg.Bits.auto_stop = 1;
    }                                 //Only for SDIO data transfer
    //Sets Memory or IO transfer.
    else
    {
        stCmdInfo.CmdReg.Bits.auto_stop = 0x0;
    }

    /* User settable.uln all modes,this field should be actual card number. */
    stCmdInfo.CmdReg.Bits.card_number = (UINT32)slot;

    stCmdInfo.CmdReg.Bits.wait_prvdata_complete = (((UINT32)flags >> 1) &
                                                   0x01);

    stCmdInfo.CmdReg.Bits.check_response_crc = (((UINT32)flags >> 2) &
                                                0x01);
    if ((cmd_id == SDIO_RESET) || (cmd_id == SDIO_ABORT))
    {
        stCmdInfo.CmdReg.Bits.cmd_index = CMD52;
    }
    else
    {
        stCmdInfo.CmdReg.Bits.cmd_index = (UINT32)cmd_id;
    }

    /* Send command */
    retval = (UINT32)emmc_send_cmd_to_host(pstGlbVar, &stCmdInfo, pucCmdRespBuff,
                                 NULL, //No data response buffer
                                 0, //No flags from user
                                 0, //No data command
                                 0);

    return retval;
}

/*****************************************************************************
* 函 数 名  : emmc_send_acmd41
*
* 功能描述  : 发送ACMD41命令
*
* 输入参数  : INT32 slot
*             BSP_UINT32 UINT32OCR
*             UINT8 *pucR3Response
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_send_acmd41(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 UINT32OCR, UINT8 *pucR3Response)
{
    UINT32 RetVal;
    UINT32 ulValue = 0;

    /* Send CMD55 first. */
    if (0 == (RetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD55, 0, (UINT8*)&ulValue, 1)))
    {
        /* Send ACMD41. */
        RetVal = emmc_form_send_cmd(slot, pstGlbVar, ACMD41, UINT32OCR, pucR3Response, 0);
    }

    return RetVal;
}

BOOL emmc_get_card_present(UINT32 slot)
{
    /* Read the CDETECT bit 0 => card connected. */
    return (INREG32(EMMC_REG_CDETECT) & (1 << slot))?0:1;
}

UINT32 emmc_set_idle(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 cnt;
    UINT32 ulRetVal;
    R1_RESPONSE_U unR1;

    for (cnt = 0; cnt < CMD0_RETRY_COUNT; cnt++)
    {
        ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD0, 00, (UINT8*)(&unR1.ulStatusReg), 1);
        if((ERRNOERROR == ulRetVal) || (ERRHLE != ulRetVal))
        {
            break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

    if ((CMD0_RETRY_COUNT <= cnt) || ulRetVal)
    {
        return ERRRETRYTIMEOUT;
    }

    return ERRNOERROR;
}

UINT32 emmc_set_preidle(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 cnt;
    UINT32 ulRetVal;
    R1_RESPONSE_U unR1;

    for (cnt = 0; cnt < CMD0_RETRY_COUNT; cnt++)
    {
        ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD0, 0xF0F0F0F0, (UINT8*)(&unR1.ulStatusReg), 1);
        if ((ERRNOERROR == ulRetVal) || (ERRHLE != ulRetVal))
        {
            break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

    if ((CMD0_RETRY_COUNT <= cnt) || (ulRetVal))
    {
        return ERRRETRYTIMEOUT;
    }

    return ERRNOERROR;
}

UINT32 emmc_get_cid(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
	UINT32 ulRetVal = 0;
	UINT32 cnt;
	R2_RESPONSE_S stR2;

	for (cnt=0; cnt<CMD2_RETRY_COUNT; cnt++)
    {
    	ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD2, 0, stR2.cResp, 1);
    	if((ERRNOERROR == ulRetVal) || (ERRHLE != ulRetVal))
        {
        	break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

	if (CMD2_RETRY_COUNT <= cnt)
    {
    	return ERRHARDWARE;
    }

    if(ERRNOERROR == ulRetVal)
    {
        ulRetVal = emmc_validate_response(slot,  pstGlbVar,CMD2, stR2.cResp, EMMC_DONOT_COMPARE_STATE);
    }

	return ulRetVal;
}


/*****************************************************************************
* 函 数 名  : emmc_validate_sd_cmd3_response
*
* 功能描述  : 检查CMD3的响应是否正确
*
* 输入参数  : BSP_S32 s32CardStatus
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_validate_sd_cmd3_response(UINT32 ulCardStatus)
{
    UINT32 u32State;

    /* Check if bits 13,14,15 are set.Error bits 19,22,23 in card
       status register. */
    if (ulCardStatus & 0xe000)
    {
        return ERRRESPRECEP;
    }

    /* Check if card is in identification state. */
    u32State = (ulCardStatus & 0x1e00) >> 9;
    if (u32State != 2)
    {
        return ERRRESPRECEP;
    }

    return ERRNOERROR;
}

UINT32 emmc_set_sd_rca(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar ,UINT32 ulRca)
{
    UINT32 cnt = 0;
    UINT32 ulRetVal = 0;
    R6_RESPONSE_U unR6;

    for (cnt = 0; cnt < CMD3_RETRY_COUNT; cnt++)
    {
        ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD3, ulRca, (UINT8*)(&unR6.reg), 1);
        if((ERRNOERROR == ulRetVal) || (ERRHLE != ulRetVal))
        {
            break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

    /* validate response.Card should be in Identification state
       This response is not same as R1.Card status register is not
       fully received in response.
       If CMD3 failed or no valid response for CMD3 then,card is not
       enumerated properly.return error. */
    if(ERRNOERROR == ulRetVal)
    {
        if (!(ulRetVal = emmc_validate_sd_cmd3_response(unR6.Bits.card_status)))
        {
            pstGlbVar->stCardInfo[slot].card_rca = unR6.Bits.card_rca;
        }
    }

	return ulRetVal;
}

UINT32 emmc_set_sd_voltage_range(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal = 0;
    UINT32 ulResp = 0;
    UINT32 ulNewOcr = 0;
    UINT32 i = 0;

    ulNewOcr = OCR_27TO36 | OCR_POWER_UP_STATUS;
    if(CARD_TYPE_SD2 == pstGlbVar->stCardInfo[slot].card_type)
    {
        ulNewOcr |= OCR_HCS;
    }

    for(i = 0; i < ACMD41_RETRY_COUNT; i++)
    {
        ulRetVal = emmc_send_acmd41(slot, pstGlbVar, ulNewOcr, (UINT8*)&ulResp);
        if ((ERRNOERROR == ulRetVal) && (ulResp & OCR_POWER_UP_STATUS))
        {
            break;
        }

        /* Delay a little time */
        emmc_delay_ms( EMMC_CMD_DISTANCE ); /* Just wailt a little time, the time is not defined in spec */
    }

//    if((cnt >= ACMD41_RETRY_COUNT) || (OCR_27TO36 != (ulResp & OCR_27TO36)))
    if((i >= ACMD41_RETRY_COUNT) || (!(ulResp & OCR_27TO36)))
    {
        return ERRHARDWARE;
    }

    if(ulResp & OCR_CCS)
    {
        pstGlbVar->stCardInfo[slot].card_type = CARD_TYPE_SD2_1;
    }

    return ERRNOERROR;
}

/*****************************************************************************
* 函 数 名  : SD_MMC_SDIO_initialise_SD
*
* 功能描述  : initialise SD card after card identify.
*
* 输入参数  : INT32 slot: Port no.
*             BSP_UINT32 UINT32RCA: card RCA
*             INT32 *s32CardType:card type
* 输出参数  :
*
* 返 回 值  : ERRNOERROR
              SDIO_ERR_ENUM_CMD0_FAILED
              SDIO_ERR_ENUM_ACMD41_FAILED
              SDIO_ERR_ENUM_CMD2_FAILED
              SDIO_ERR_COMMAND_NOT_ACCEPTED
              SDIO_ERR_ENUM_CMD3_FAILED
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_enumerate_sd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulRCA)
{
    UINT32 ulRetVal = ERRNOERROR;

    if((ulRetVal = emmc_set_sd_voltage_range(slot,pstGlbVar)))
    {
        return ulRetVal;
    }

    if((ulRetVal = emmc_get_cid(slot,pstGlbVar)))
    {
        return ulRetVal;
    }

    if((ulRetVal = emmc_set_sd_rca(slot,pstGlbVar,ulRCA)))
    {
        return ulRetVal;
    }

	if((ulRetVal = emmc_process_csd(slot,pstGlbVar,FALSE)))
    {
    	return ulRetVal;
    }

	return ulRetVal;
} /* End of SD_MMC_SDIO_initialise_SD(). */

/**
  * Sets the voltage for the connected MMC card.
  * This function is modified inorder to incorporate the MMC4.2 high capacity card's voltage
  * setting also. OCR_ACCESSMODE_SECTOR field is set in the argument of CMD1 indicating
  * host supports the sector addressing. But the card's addressing capability is validated
  * only after reading the SEC_COUNT field of EXT_CSD register.
  * This will flag an error if the card is
  * not in the IDLE state It does the following --
  *    -# Check if any card is present in the slot
  *    -# Check if the card is in the idle state
  *    -# Set (CMD1) the OCR to the MMC range.
  *    -# If there is a failure in the voltage setting, mark the card as
  *	inactive.
  * @param[in] slot The slot in which the card is
  * \return Returns 0 upon succes and error status upon failure
  */
UINT32 emmc_set_mmc_voltage_range(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal = 0;
    UINT32 cnt= 0;
    UINT32 ulResp = 0;
    UINT32 new_ocr = 0;
    R2_RESPONSE_S stR2;

    /* the new_ocr is modified to incorporate access mode to indicate sector addressing capability of the host
      The modification to this function should not create any problem to MMC4.1 and earlier cards as OCR_ACCESSMODE_SECTOR
      bit fields are reserverd and card should not care for the value here	*/
    new_ocr = OCR_27TO36 | OCR_POWER_UP_STATUS | OCR_ACCESSMODE_SECTOR;
    for(cnt = 0; cnt < CMD1_RETRY_COUNT; cnt++)
    {
    	ulResp = 0;
    	ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD1, new_ocr, (UINT8 *)&ulResp, 1);
    	if ((ulResp & OCR_POWER_UP_STATUS) && (!ulRetVal))
        {
            /* The power up process for the card is over */
        	break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

    if ((cnt >= CMD1_RETRY_COUNT) || ulRetVal)
    {
        return ERRHARDWARE;
    }

    if(!(ulResp&OCR_27TO36))
    {
        return ERRHARDWARE;
    }

    if( ulResp & OCR_ACCESSMODE_SECTOR )
    {
        pstGlbVar->stCardInfo[slot].card_type = CARD_TYPE_MMC_42;

    }

    ulRetVal = emmc_validate_response(slot,pstGlbVar,CMD1, &stR2.cResp[0], EMMC_DONOT_COMPARE_STATE);
	return ulRetVal;
}

UINT32 emmc_set_rca(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 rca)
{
    UINT32 cnt;
    UINT32 ulRetVal;
    UINT32 u32Rca;
    R6_RESPONSE_U unR6;

	u32Rca = (rca + 1) << 16;
    for (cnt = 0; cnt < CMD3_RETRY_COUNT; cnt++)
    {
        ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD3, u32Rca, (UINT8*)(&unR6.reg), 1);
        if ((ERRNOERROR == ulRetVal) || (ERRHLE != ulRetVal))
        {
            break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

    if(ERRNOERROR == ulRetVal)
    {
        pstGlbVar->stCardInfo[slot].card_rca = slot + 1;
    }

	return ulRetVal;
}

UINT32 emmc_process_csd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, BOOL bMmcType)
{
    UINT32 ulRetVal;
    R2_RESPONSE_S stR2;

    if((ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD9, 00, stR2.cResp, 1)))
    {
        return ulRetVal;
    }

    (VOID)emmc_validate_response(slot,pstGlbVar,CMD9, stR2.cResp, EMMC_DONOT_COMPARE_STATE);

    /* Store maximum TAAC and NSAC values for timeout calculation. */
    if (pstGlbVar->stCardInfo[slot].CSD.Fields.taac > pstGlbVar->stIPStatusInfo.max_tacc_value)
    {
        pstGlbVar->stIPStatusInfo.max_tacc_value = pstGlbVar->stCardInfo[slot].CSD.Fields.taac;
    }

    if (pstGlbVar->stCardInfo[slot].CSD.Fields.nsac > pstGlbVar->stIPStatusInfo.max_msac_value)
    {
        pstGlbVar->stIPStatusInfo.max_msac_value = pstGlbVar->stCardInfo[slot].CSD.Fields.nsac;
    }

    /* read/write block size */
    pstGlbVar->stCardInfo[slot].card_read_blksize =
        ((0x0001 << (pstGlbVar->stCardInfo[slot].CSD.Fields.read_bl_len)) > 512) ? 512 :
        (0x0001 << (pstGlbVar->stCardInfo[slot].CSD.Fields.read_bl_len));

    pstGlbVar->stCardInfo[slot].card_write_blksize =
        ((0x0001 << (pstGlbVar->stCardInfo[slot].CSD.Fields.write_bl_len)) > 512) ? 512 :
        (0x0001 << (pstGlbVar->stCardInfo[slot].CSD.Fields.write_bl_len));

    if(!bMmcType)
    {
        /* Calculate and store total card size in bytes */
        if (pstGlbVar->stCardInfo[slot].CSD.Fields.csd_structure)    /* CSD version 2.0 */
        {
            pstGlbVar->stCardInfo[slot].card_size = ((UINT64)pstGlbVar->stCardInfo[slot].CSD.Fields.c_size + 1) * 1024;
        }
        else    /* CSD version 1.0 */
        {
            pstGlbVar->stCardInfo[slot].card_size = (1 + (UINT64)pstGlbVar->stCardInfo[slot].CSD.Fields.c_size) *
                (0x1 << (pstGlbVar->stCardInfo[slot].CSD.Fields.c_size_mult + 2)) *    /*lint !e647*/
                (0x1 << (pstGlbVar->stCardInfo[slot].CSD.Fields.read_bl_len))/512;     /*lint !e647*/
        }
    }
    else
    {
        pstGlbVar->stCardInfo[slot].card_size = (1 + (UINT64)pstGlbVar->stCardInfo[slot].CSD.Fields.c_size) *
           (0x01 << (pstGlbVar->stCardInfo[slot].CSD.Fields.c_size_mult + 2)) *  /*lint !e647 */
            (0x1 << (pstGlbVar->stCardInfo[slot].CSD.Fields.read_bl_len))/512;     /*lint !e647*/
    }

	return ERRNOERROR;
}

UINT32 emmc_read_extcsd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT8 *pcExtCSD)
{
    COMMAND_INFO_S  stCmdInfo;
    UINT32 ulR1Resp = 0x0;
    UINT32 i = 0;
    UINT32 ulRetVal = ERRCOMERR;

    for(i = 0; i < 512; i++)
    {
        *(pcExtCSD + i) = 0;
    }

    stCmdInfo.CmdArg      = 0;
    stCmdInfo.ByteCnt     = 512;  /* The Ext CSD is 512 Bytes */
    stCmdInfo.BlkSize     = EMMC_SEC_SIZE;
    stCmdInfo.CmdReg.reg  = EMMC_CMD_BIT_START
                            | EMMC_CMD_BIT_WAIT_PRV_COMPLETE
                            | EMMC_CMD_BIT_DATA_EXPECTED
                            | EMMC_CMD_BIT_CHECK_RESP_CRC
                            | EMMC_CMD_BIT_RESP_EXPECTED /* short response */
                            | EMMC_CMD8;  /* The Card send it's Ext_CSD, the response is R1 */

    ulRetVal = emmc_send_cmd_to_host(pstGlbVar, &stCmdInfo, (UINT8 *)&ulR1Resp, pcExtCSD, 0, 1, 0);
    if(ulRetVal)
    {
        print_info("\r\nGet Ext_CSD fail");
        return ulRetVal;
    }

    /*
        If command executed properly,Check response for any error
        reported by card. Previous state should be Transfer state.
    */
    ulRetVal = emmc_validate_response(slot, pstGlbVar,stCmdInfo.CmdReg.reg & 0x3F, /* get cmd index */
                               (UINT8 *)&ulR1Resp, EMMC_CARD_STATE_TRAN);
    return ulRetVal;
}
UINT32 emmc_write_extcsd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT8 ucIndex, UINT8 ucValue, UINT8 ucAccess)
{
    COMMAND_INFO_S  stCmdInfo;
    UINT32 ulR1Resp = 0x0;
    UINT32 ulRetVal = ERRCOMERR;


    /*
        [31：26]: set to 0
        [25:24] : Access (00:command set. 01:Set Bits. 10:Clear Bits. 11:Write Byte)
        [23:16] : Index
        [15:8]  : value
        [7:3]   : Set to 0
        [2:0]   : Cmd Set (000 for write ext_csd )
    */
    stCmdInfo.CmdArg = (ucAccess << 24)
                        |((((UINT32)ucIndex) << 16) & 0x00FF0000)
                        |((((UINT32)ucValue) << 8) & 0x0000FF00)
                        |0x0;   /* Bit 0-7 */

    stCmdInfo.ByteCnt = 0;
    stCmdInfo.BlkSize = 0;
    stCmdInfo.CmdReg.reg = EMMC_CMD_BIT_START
                            | EMMC_CMD_BIT_WAIT_PRV_COMPLETE
                            | EMMC_CMD_BIT_CHECK_RESP_CRC
                            | EMMC_CMD_BIT_RESP_EXPECTED
                            | CMD6;  /* Switch, response is R1b */

    ulRetVal = emmc_send_cmd_to_host(pstGlbVar, &stCmdInfo, (UINT8 *)&ulR1Resp, 0, 0, 0, 0);
    if(ulRetVal)
    {
        print_info("\r\nwrite extcsd send cmd fail");
        return ulRetVal;
    }

    /*
        If command executed properly,Check response for any error
        reported by card. Previous state should be Transfer state.
        Not check the state, because the state is tran or prog(if not ready)
    */
    ulRetVal = emmc_validate_response(slot, pstGlbVar,stCmdInfo.CmdReg.reg & 0x3F, /* get cmd index */
                               (UINT8 *)&ulR1Resp,
                               EMMC_DONOT_COMPARE_STATE );

    if(ulRetVal)
    {
        print_info("\r\nwrite extcsd valid resp fail");
        return ulRetVal;
    }

    ulRetVal = emmc_wait_data0_ready();
    if(ulRetVal)
    {
        print_info("\r\nwrite extcsd wait ready timeout");
    }

    return ERRNOERROR;
}

UINT32 emmc_process_extcsd(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal = ERRCOMERR;
    UINT8 arrExtCSD[512];

    ulRetVal = emmc_check_transtate(slot,pstGlbVar);
    if(ulRetVal)
    {
        return ulRetVal;
    }

    if(!(ulRetVal = emmc_read_extcsd(slot,pstGlbVar,arrExtCSD)))
    {
        pstGlbVar->stCardInfo[slot].extcsd_bootinfo = arrExtCSD[228];    /* readonly */
        pstGlbVar->stCardInfo[slot].extcsd_bootsizemult = arrExtCSD[226];/* readonly */
        pstGlbVar->stCardInfo[slot].extcsd_seccount = emmc_byte2ulong(arrExtCSD+212);/* readonly */
        if(CARD_TYPE_MMC_42 == pstGlbVar->stCardInfo[slot].card_type)
        {
            pstGlbVar->stCardInfo[slot].card_size = pstGlbVar->stCardInfo[slot].extcsd_seccount;
        }
        pstGlbVar->stCardInfo[slot].extcsd_rev = arrExtCSD[192];
        pstGlbVar->stCardInfo[slot].extcsd_partitioncfg = arrExtCSD[179];
        pstGlbVar->stCardInfo[slot].extcsd_bootbuswidth = arrExtCSD[177];

        if(EXTCSD_BOOT_PARTITION_1 == (pstGlbVar->stCardInfo[slot].extcsd_partitioncfg&EXTCSD_BOOT_PARTITION_ENABLE_MSK))
        {
            if(EXTCSD_BOOT_BUS_WIDTH_1==(pstGlbVar->stCardInfo[slot].extcsd_bootbuswidth&EXTCSD_BOOT_BUS_WIDTH_MASK))
            {
                pstGlbVar->stCardInfo[slot].extcsd_bootop = TRUE;
                if(pstGlbVar->stCardInfo[slot].extcsd_bootbuswidth&EXTCSD_BOOT_BUS_OTHER_MASK)
                {
                    print_info_with_u32("\r\nplease set proper bootbuswidth!cur=0x",pstGlbVar->stCardInfo[slot].extcsd_bootbuswidth);
                }
            }
        }
    }

    return ulRetVal;
}

/*****************************************************************************
* 函 数 名  : enumerate_mmc_card
*
* 功能描述  : Enumerates single MMC card.
*
* 输入参数  : BSP_S32 slot:Card number.
*             BSP_S32 slot:Port number.Enumearte card attached to this port.
*             UINT32 u32StartRCA:Relative card address to be assigned to card.
* 输出参数  : peCardType
*
* 返 回 值  : 无
*
* 其它说明  : Always this will enumerate only one card.
*
*****************************************************************************/
UINT32 emmc_enumerate_mmc(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulStartRCA)
{
    UINT32 ulRetVal = ERRNOERROR;

    emmc_config_od(TRUE);

    if((ulRetVal = emmc_set_idle(slot,pstGlbVar)))
    {
        ulRetVal = ERRENUMERATE;
        goto housekeep;
    }

    if((ulRetVal = emmc_set_mmc_voltage_range(slot,pstGlbVar)))
    {
        ulRetVal = ERRENUMERATE;
        goto housekeep;
    }

    /* Card is in READY state. Can now extract the CID */
	if((ulRetVal = emmc_get_cid(slot,pstGlbVar)))
    {
    	ulRetVal = ERRENUMERATE;
        goto housekeep;
    }

    /* Card is in IDENT state. Now finally set the RCA */
	if((ulRetVal = emmc_set_rca(slot,pstGlbVar,ulStartRCA)))
    {
    	ulRetVal = ERRENUMERATE;
        goto housekeep;
    }

    emmc_config_od(FALSE);

    /* Card is in STANDBY. */
	if((ulRetVal = emmc_process_csd(slot,pstGlbVar,TRUE)))
    {
    	ulRetVal = ERRENUMERATE;
        goto housekeep;
    }

    if(pstGlbVar->stCardInfo[slot].CSD.Fields.spec_vers >= CSD_SPEC_VER_4)
    {
        if((ulRetVal = emmc_process_extcsd(slot,pstGlbVar)))
        {
            ulRetVal = ERRENUMERATE;
            goto housekeep;
        }
    }

housekeep:
    emmc_config_od(FALSE);
	return ulRetVal;
}

UINT32 emmc_enumerate_the_card(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
	CARD_TYPE_E type;
	UINT32 ulRetVal = 0;
	UINT32 ulRCA = slot;

    /* 上电之后才能检测卡是否存在，对emmc方式无影响 */
	if(!emmc_get_card_present(slot))
    {
        print_info("\r\nno card insert!");
        return ERRCARDNOTCONN;
    }

    if((ulRetVal = emmc_enable_clock(slot,pstGlbVar)))
    {
        return ulRetVal;
    }

    /* To start with, without knowing what kind of card/drive start enumeration with the lowest clk	*/
    if((ulRetVal = emmc_set_clk_freq(slot, pstGlbVar, EMMC_CURRENT_CLKSRC,DEFAULT_FOD_DIVIDER_VALUE)))
    {
        return ulRetVal;
    }

    /* Lets start with the single bit mode initially */
	CLRREG32(EMMC_REG_CTYPE, (UINT32)((1<<slot)|(1<<(slot + 16))));

#ifdef EMMC_DEBUG
    emmc_reg_dump();
#endif

    type = emmc_identify_card_type(slot,pstGlbVar);
    pstGlbVar->stCardInfo[slot].card_type = type;
    /* Call corresponding initialisation routine. */
    switch (type)
    {
        case CARD_TYPE_SD:
        case CARD_TYPE_SD2:
            ulRetVal = emmc_enumerate_sd(slot, pstGlbVar, ulRCA);
            break;

        case CARD_TYPE_MMC:
        case CARD_TYPE_MMC_42:
            ulRetVal = emmc_enumerate_mmc(slot, pstGlbVar, ulRCA);
            break;

        default:
            print_info_with_u32("\r\nidentify unknow card:0x",type);
            return ERRCARDNOTFOUND;
    }

	return ulRetVal;
}

CARD_TYPE_E emmc_identify_card_type(UINT32 slot,EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal;
    UINT32 ulResp[4]={0,0,0,0};
    UINT32 i = 0;

    if((ulRetVal = emmc_set_idle(slot,pstGlbVar)))
    {
        return CARD_TYPE_INVALID;
    }

    /* Send CMD8 to differentiate between SD and MMC. */
    for(i=0; i<EMMC_CMD8_RETRY_COUNT; i++)
    {
        ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD8, 0x1AA, (UINT8 *)ulResp, 1);
        if((ERRNOERROR == ulRetVal)||(ERRRESPTIMEOUT == ulRetVal))
        {
            break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

    if(ERRNOERROR == ulRetVal)
    {
        return CARD_TYPE_SD2;
    }
    else if(ERRRESPTIMEOUT != ulRetVal)
    {
        return CARD_TYPE_INVALID;
    }
    else
    {
#ifdef EMMC_DEBUG
        print_info("\r\nCMD8 timeout...");
#endif
    }

    /* Send ACMD41 to differentiate between SD1.0 and MMC. */
    for(i=0; i<EMMC_ACMD41_RETRY_COUNT; i++)
    {
        ulRetVal = emmc_send_acmd41(slot, pstGlbVar, 0, (UINT8 *)ulResp);
        if((ERRNOERROR == ulRetVal)||(ERRRESPTIMEOUT == ulRetVal))
        {
            break;
        }

        /* Delay a little time */
        emmc_delay_ms(EMMC_CMD_DISTANCE); /* Just wailt a little time, the time is not defined in spec */
    }

    if(ERRNOERROR == ulRetVal)
    {
        return CARD_TYPE_SD;
    }
    else if(ERRRESPTIMEOUT != ulRetVal)
    {
        return CARD_TYPE_INVALID;
    }
    else
    {
#ifdef EMMC_DEBUG
        print_info("\r\nACMD41 timeout...");
#endif
    }

    return CARD_TYPE_MMC;
}

/*****************************************************************************
* 函 数 名  : SD_set_mode
*
* 功能描述  : for SD/SDIO card.:Since,this is not frequently changed by user,
              it is not added as a command parameter and provided as a seperate
              function.

              Sets mode of operation for SD/SDIO cards.Writes into card
              registers @0x018.
*
* 输入参数  : BSP_S32 slot:Card number whose mode is to be set.
*             BSP_S32 s32Mode4Bit:Mode . 0=1-bit mode, 1=4-bit mode.
* 输出参数  :
*
* 返 回 值  : ERRNOERROR
              SDIO_ERR_INVALID_CARDNO
              SDIO_ERR_CMDINVALID_FORCARDTYPE
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_set_sd_bus_width(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, BOOL bMode4Bit)
{
    UINT32 ulRetVal;
    UINT32 u32Arg;
    R1_RESPONSE_U unR1;

    if ((CARD_TYPE_MMC == pstGlbVar->stCardInfo[slot].card_type)
        ||(CARD_TYPE_MMC_42 == pstGlbVar->stCardInfo[slot].card_type))
    {
        return ERRNOTSUPPORTED;
    }

    /* Send ACMD6 command to card to set bus width. */
    ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD55, 0, (UINT8*)(&unR1.ulStatusReg), 1);
    if (ulRetVal)
    {
        return ERRCMDNOTSUPP;
    }

    /* Card is in transfer state.Now send ACMD6.
       For one bit mode,arg.should be 00. */
    u32Arg = bMode4Bit?0x02:0x0;
    ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD6, u32Arg, (UINT8*)(&unR1.ulStatusReg), 00);
    if (ulRetVal)
    {
        return ulRetVal;
    }

    /* Validate response. */
    ulRetVal = emmc_validate_response(slot, pstGlbVar,CMD6, (UINT8*)&unR1.ulStatusReg, CARD_STATE_TRAN);
    if (ulRetVal)
    {
        return ulRetVal;
    }

    /* now set the width of the bus */
	CLRREG32(EMMC_REG_CTYPE, (UINT32)(1<<slot));
	CLRREG32(EMMC_REG_CTYPE, (UINT32)(slot + 16));
	if(bMode4Bit)
    {
        SETREG32(EMMC_REG_CTYPE, (UINT32)(1<<slot));
    }

    return ERRNOERROR;
}

/*****************************************************************************
* 函 数 名  : emmc_check_transtate
*
* 功能描述  : Checks if the given card is in transfer state or not.
*
* 输入参数  : BSP_S32 slot:Card number.
* 输出参数  :
*
* 返 回 值  : ERRNOERROR:card is in transfer mode.
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_check_transtate(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulR1Resp, ulCardStatus;
    UINT32 ulRetVal;

    /* Check card status. */
    ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD13, 00, (UINT8*)&ulR1Resp, 1);
    if (ulRetVal)
    {
        return ulRetVal;
    }

    /* If card state = STDBY,issue CMD7.
       If card state = TRANS,do not issue CMD7.Card is selected.
       For other states,return error. */
    ulCardStatus = R1_CURRENT_STATE(ulR1Resp);
    if (ulCardStatus == CARD_STATE_STBY)
    {
        /* Send card in transfer mode.CMD7. */
        if ((ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD7, 00, (UINT8*)&ulR1Resp,1)))
        {
            return ulRetVal;
        }

        /* Validate response. */
        if ((ulRetVal = emmc_validate_response(slot, pstGlbVar,CMD7, (UINT8*)&ulR1Resp, EMMC_DONOT_COMPARE_STATE)))
        {
            return ulRetVal;
        }
    }
    else if (ulCardStatus != CARD_STATE_TRAN)
    {
        return ERRFSMSTATE;
    }

    return ERRNOERROR;
}

/*****************************************************************************
* 函 数 名  : form_data_command
*
* 功能描述  :
*
* 输入参数  : UINT32 slot
*             UINT32 ulAddr
*             UINT32 ulSize
*             BSP_S32 ulFlags
*             COMMAND_INFO_S *pstCmdInfo
*             BSP_S32 ulTransferMode
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
UINT32 emmc_form_data_command(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulAddr,
                          UINT32 ulSize, UINT32 ulFlags,
                          COMMAND_INFO_S *pstCmdInfo,
                          UINT32 ulTransferMode)
{
    //start_cmd
    pstCmdInfo->CmdReg.Bits.start_cmd = 1;

    //Update_clk_regs_only.
    pstCmdInfo->CmdReg.Bits.Update_clk_regs_only = 0;

    //card no.
    pstCmdInfo->CmdReg.Bits.card_number = (UINT32)slot;

    //send_initialization or not (bit0)
    pstCmdInfo->CmdReg.Bits.send_initialization = (ulFlags & 0x01);

    //Stop_abort command.
    pstCmdInfo->CmdReg.Bits.stop_abort_cmd = 0;

    //wait till previous data complete
    pstCmdInfo->CmdReg.Bits.wait_prvdata_complete = (((UINT32)ulFlags >> 1) & 0x01);

    //This command is always used for memory transfer.
    //SDIO_IO transfer will not use this.
    pstCmdInfo->CmdReg.Bits.auto_stop = 0; //no auto_stop

    //Set transfer_mode. 1=stream, 0=block.
    pstCmdInfo->CmdReg.Bits.transfer_mode = (UINT32)ulTransferMode;

    //Read/write. bit3
    pstCmdInfo->CmdReg.Bits.read_write = (((UINT32)ulFlags >> 7) & 0x01);

    //Data always expected for data transfer.(RD/WR)
    pstCmdInfo->CmdReg.Bits.data_expected = 1;

    //respose crc to check or not
    pstCmdInfo->CmdReg.Bits.check_response_crc =
        (((UINT32)ulFlags >> 2) & 0x01);

    //short response
    pstCmdInfo->CmdReg.Bits.response_length = 0;

    //Response always expected.
    pstCmdInfo->CmdReg.Bits.response_expect = 1;

    //Set command argument register.
    pstCmdInfo->CmdArg = ulAddr;

    //Set UINT8 count register value.
    pstCmdInfo->ByteCnt = ulSize;

    //Set Block size register value.This value is ignored in
    //stream transfer.
    if (pstCmdInfo->CmdReg.Bits.read_write)
    {
        pstCmdInfo->BlkSize = pstGlbVar->stCardInfo[slot].card_write_blksize;
    }
    else
    {
        pstCmdInfo->BlkSize = pstGlbVar->stCardInfo[slot].card_read_blksize;
    }

    return ERRNOERROR;
}

/*****************************************************************************
* 函 数 名  : set_auto_stop
*
* 功能描述  : Condition for auto stop
*             For STREAM READ mode  : bytecount should be > 22
*             For STREAM WRITE mode : bytecount should be > 6
*             For BLOCK READ  mode, 1 BIT :
*               (bytecount > 20) && (BLOCKsize > 6)
*             For BLOCK WRITE  mode, 1 BIT :
*               (bytecount > 6) && (BLOCKsize > 6)
*             For BLOCK READ  mode, 4 BIT :
*               (bytecount > 80) && (BLOCKsize >24)
*             For BLOCK WRITE  mode,4 BIT :
*               (bytecount > 24) && (BLOCKsize >24)

*             Formula for block transfers:
*               1bit ? mult=4 :1. For MMC block transfer,always 1 bit transfer.
*               if (read && (blksize*mult >24) && (bytecount * mult >80)),Set Autostop.
*               if (write && (blksize*mult >24) && (bytecount * mult >24)),Set Autostop.
*             todo: I don't know where the precede description come from?
*
* 输入参数  : BSP_S32 slot
*             COMMAND_INFO_S *pstCmdInfo
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
VOID set_auto_stop(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, COMMAND_INFO_S *pstCmdInfo)
{
    UINT32 ulBlkSize, ulBytCnt, ulCtype = 0;
    UINT32 ulMult = 0;

    /* read card type register.check required sd width bit. */
    ulCtype = INREG32(EMMC_REG_CTYPE);

    ulCtype &= 0x0000ffff;

    /* set multiplier. */
    ulMult = ((ulCtype >> slot) & 1) ? 1 : 4;

    /* MMC is always 1bit.So,multiplier is 4 always. */
    if((CARD_TYPE_MMC == pstGlbVar->stCardInfo[slot].card_type)
        ||(CARD_TYPE_MMC_42 == pstGlbVar->stCardInfo[slot].card_type))
    {
        ulMult = 4;
    }

    /* Set auto stop bit. */
    ulBlkSize = pstCmdInfo->BlkSize;
    ulBytCnt = pstCmdInfo->ByteCnt;

    pstCmdInfo->CmdReg.Bits.auto_stop = 0;

    if (((pstCmdInfo->CmdReg.Bits.read_write)    /* write */
         && ((ulBlkSize * ulMult) > 24)
         && ((ulBytCnt * ulMult) > 24)
        )||(!(pstCmdInfo->CmdReg.Bits.read_write)   /* read */
         && ((ulBlkSize * ulMult) > 24)
         && ((ulBytCnt * ulMult) > 80))
    )
    {
        pstCmdInfo->CmdReg.Bits.auto_stop = 1;
    }

    return;
}

/*****************************************************************************
* 函 数 名  : emmc_blksize_data_transfer
*
* 功能描述  : 整块传输(起始地址和数据个数为块大小的整数倍)
*
* 输入参数  : BSP_S32 slot
*             UINT32 ulAddr
*             UINT32 ulSize
*             UINT8  *pucDataBuff
*             UINT8  *pucCmdRespBuff
*             UINT8  *pucErrRespBuff
*             BSP_VOID (*callback)(UINT32 u32ErrData)
*             BSP_S32 s32NoOfRetries
*             BSP_S32 ulFlags
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
UINT32  emmc_blksize_data_transfer(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulAddr,
                               UINT32 ulSize,
                               UINT8  *pucDataBuff,
                               UINT8  *pucCmdRespBuff,
                               UINT8  *pucErrRespBuff,
                               UINT32 ulFlags)
{
    UINT32 ulRetVal;
    COMMAND_INFO_S stCmdInfo;

    memset(&stCmdInfo,0x0,sizeof(COMMAND_INFO_S));

    //ulBlockSize =(ulFlags & 0x080)?pstGlbVar->stCardInfo[slot].card_write_blksize:pstGlbVar->stCardInfo[slot].card_read_blksize;

    (VOID)emmc_form_data_command(slot, pstGlbVar, ulAddr, ulSize, ulFlags,&stCmdInfo, 00);

    /* Set block transfer specific command index:cmd24/25/17/18. */
    if (ulFlags & 0x080) /* write */
    {
        if (ulSize <= pstGlbVar->stCardInfo[slot].card_write_blksize)
        {
            /* Set cmd_index for Single blk transfer. */
            stCmdInfo.CmdReg.Bits.cmd_index = CMD24;
        }
        else
        {
            /* Set cmd_index for Multiple blk transfer. */
            stCmdInfo.CmdReg.Bits.cmd_index = CMD25;
        }
    }
    else    /* read */
    {
        if (ulSize <= pstGlbVar->stCardInfo[slot].card_read_blksize)
        {
            /* Set cmd_index for Single blk transfer. */
            stCmdInfo.CmdReg.Bits.cmd_index = CMD17;
        }
        else
        {
            /* Set cmd_index for Multiple blk transfer. */
            stCmdInfo.CmdReg.Bits.cmd_index = CMD18;
        }
    }

    if ((CMD25 == stCmdInfo.CmdReg.Bits.cmd_index)
        && ((CARD_TYPE_SD <= pstGlbVar->stCardInfo[slot].card_type)
        &&(CARD_TYPE_SD2_1 >= pstGlbVar->stCardInfo[slot].card_type)))
    {
#if 0
        R1_RESPONSE_U unR1;

        /* 在多块写之前发送ACMD23命令进行预擦除 ,以加快数据写操作*/
        if (ERRNOERROR == (ulRetVal = emmc_form_send_cmd(slot, pstGlbVar, CMD55, 00,
                                                 (UINT8*)(&unR1.ulStatusReg), 1)))
        {
            ulRetVal = emmc_form_send_cmd(slot, CMD23, (ulSize/(UINT32)ulBlockSize), (UINT8*)(&unR1.ulStatusReg), 00);
            if (ulRetVal)
            {
                print_info("emmc_blksize_data_transfer:send CMD23 failed!\n");
            }
        }
#endif
    }

    if (CMD25 == (stCmdInfo.CmdReg.Bits.cmd_index)   /* multi-block write */
        || (CMD18 == stCmdInfo.CmdReg.Bits.cmd_index))   /* multi-block read */
    {
        set_auto_stop(slot, pstGlbVar, &stCmdInfo);
    }

    /* Actually sends command to host. */
    ulRetVal = emmc_send_cmd_to_host(pstGlbVar, &stCmdInfo, pucCmdRespBuff,pucDataBuff,ulFlags,TRUE,(ulFlags & 0x080));
    if (ulRetVal)
    {
        return ulRetVal;
    }

    ulRetVal = emmc_validate_response(slot, pstGlbVar, stCmdInfo.CmdReg.Bits.cmd_index,
                                  pucCmdRespBuff, CARD_STATE_TRAN);
    return ulRetVal;
}/*lint !e715*/

UINT32 emmc_block_read(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulStartBlk, UINT8 *pucBuf, UINT32 ulLen)
{
    UINT32 ulRetVal= 0;
    BOOL bCardConnected;
    UINT32 ulSecCnt = ulLen/EMMC_SEC_SIZE;
    UINT32 u32CmdRespBuf[4] = {0};
    UINT32 u32ErrRespBuf = 0;

    /* 检查卡是否插入 */
    bCardConnected = emmc_get_card_present(slot);
    if(!bCardConnected)  /* 检查设备是否已插入 */
    {
        return ERRCARDNOTCONN;
    }

    if((ulLen%EMMC_SEC_SIZE)||(!ulLen))
    {
        return ERRPARAM;
    }

    if(((UINT64)ulStartBlk  + ulSecCnt) > pstGlbVar->stCardInfo[slot].card_size)
    {
        return ERRPARAM;
    }

    ulRetVal = read_write_blkdata(slot, pstGlbVar, ulStartBlk,ulLen,pucBuf,(UINT8 *)&u32CmdRespBuf[0],(UINT8 *)&u32ErrRespBuf,EMMC_READ_FLAG);
    return ulRetVal;
}

#ifdef EMMC_DEBUG
UINT32 emmc_block_write(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulStartBlk, UINT8 *pucBuf, UINT32 ulLen)
{
    UINT32 ulRetVal= 0;
    BOOL bCardConnected;
    UINT32 ulSecCnt = ulLen/EMMC_SEC_SIZE;
    UINT32 u32CmdRespBuf[4] = {0};
    UINT32 u32ErrRespBuf = 0;

    /* 检查卡是否插入 */
    bCardConnected = emmc_get_card_present(slot);
    if(!bCardConnected)  /* 检查设备是否已插入 */
    {
        return ERRCARDNOTCONN;
    }

    if((ulLen%EMMC_SEC_SIZE)||(!ulLen))
    {
        print_info_with_u32("\r\nemmc_block_write: data size not section align!ulLen=0x",ulLen);
        return ERRPARAM;
    }

    if(((UINT64)ulStartBlk  + ulSecCnt) > pstGlbVar->stCardInfo[slot].card_size)
    {
        print_info("\r\nemmc_block_write: offset + amount exceed cardsize");
        return ERRPARAM;
    }

    ulRetVal = read_write_blkdata(slot, pstGlbVar, ulStartBlk,ulLen,pucBuf,(UINT8 *)&u32CmdRespBuf[0],(UINT8 *)&u32ErrRespBuf,EMMC_WRITE_FLAG);
    return ulRetVal;
}
#endif

/*****************************************************************************
* 函 数 名  : read_write_blkdata
*
* 功能描述  : 发送单/多块数据.
*             1. Read open ended block data.
*             2. Read data with predefined block count.
*             如果Bytesize = BlockSize,则进行单块传输，否则为多块传输.
*
*             Since,Application info will be maintained in Interface
*             driver,Buffers need to be passed here.
*             Flags could be passed using OR condition. eg
*             Flags = Wait_PrvData||Chk_RespCrc||Preedefineed_Xfer

*             Now,BSD will be receiving one command at a time.
*             It should not store any command data/record with it as
*             next command will overwrite that.
*
* 输入参数  : BSP_S32 slot
*             UINT32 ulAddr
*             UINT32 ulSize
*             UINT8  *pucDataBuff
*             UINT8  *pucCmdRespBuff
*             UINT8  *pucErrRespBuff
*             BSP_VOID (*callback)(UINT32 u32ErrData)
*             BSP_S32 s32NoOfRetries
*             BSP_S32 ulFlags
* 输出参数  : 无
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
UINT32  read_write_blkdata(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT32 ulBlkOfs, UINT32 ulSize,
                            UINT8  *pucDataBuff,
                            UINT8  *pucCmdRespBuff,
                            UINT8  *pucErrRespBuff,
                            UINT32 ulFlags)
{
    UINT32 ulBlockSize;
    UINT32 ulAddr;
    UINT32 ulRetVal;//, ulR1Resp;

    /* Check and set card in transfer mode */
    if ((ulRetVal = emmc_check_transtate(slot,pstGlbVar)))
    {
        return ulRetVal;
    }

    ulBlockSize = (ulFlags & 0x080)?pstGlbVar->stCardInfo[slot].card_write_blksize:pstGlbVar->stCardInfo[slot].card_read_blksize;
    if((!ulBlockSize) || (ulBlockSize%EMMC_SEC_SIZE))
    {
        return ERRPARAM;
    }

    ulAddr = ((CARD_TYPE_SD2_1 == pstGlbVar->stCardInfo[slot].card_type)||(CARD_TYPE_MMC_42 == pstGlbVar->stCardInfo[slot].card_type))
                ?ulBlkOfs:(ulBlkOfs*ulBlockSize);

    /* 执行数据传输 */
    ulRetVal = (UINT32)emmc_blksize_data_transfer(slot, pstGlbVar, ulAddr,
                                      ulSize,
                                      pucDataBuff,
                                      pucCmdRespBuff,
                                      pucErrRespBuff,
                                      ulFlags);

    if (ulRetVal)    /* 数据传输失败 */
    {
        print_info_with_u32("\r\nemmc_blksize_data_transfer error!,ulRetVal=0x",ulRetVal);
        return ulRetVal;
    }

#if 0
    /* 检查命令执行时是否产生错误(Bits 19,20,21,22,23,24,26,27,29,30,31) */
    if (ERRNOERROR == (ulRetVal = emmc_form_send_cmd(slot, CMD13, 00, (UINT8*)&ulR1Resp, 1)))
    {
        /* 检查错误比特. */
        if((ulR1Resp & R1_EXECUTION_ERR_BITS)&&(pucErrRespBuff))
        {
            ulRetVal = ERRWRITE;
            memcpy(pucErrRespBuff, (UINT8*)&ulR1Resp, 4);
        }
    }
#endif

    return ulRetVal;
}

UINT32 emmc_get_bootinfo(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar, UINT8 *pucBuf, UINT32 *pulBootAddr, UINT32 *pulBootLen)
{
    UINT32 ulRetVal;
    UINT32 ulStartBlk;
#ifdef EMMC_DEBUG
    UINT32 i;
#endif

    ulStartBlk = (UINT32)(pstGlbVar->stCardInfo[slot].card_size - 1);

    ulRetVal = emmc_block_read(slot, pstGlbVar, ulStartBlk, pucBuf, EMMC_SEC_SIZE);
    if(ERRNOERROR == ulRetVal)
    {
#ifdef EMMC_DEBUG
        print_info("\r\n");
        for(i=0;i<32;i++)
        {
            print_info_with_u32(" 0x",pucBuf[i]);
            if(0 == ((i+1)%4))
            {
                print_info("\r\n");
            }
        }
#endif
        /* 获取BootLoader长度 */
        *pulBootLen = emmc_byte2ulong(pucBuf+BL_LEN_INDEX);

        /* 获取BootLoader地址(和ROOT_CA地址复用) */
        *pulBootAddr = emmc_byte2ulong(pucBuf+ROOT_CA_INDEX);

        /* 判断长度是否越界，防止翻转 */
        if((0 == *pulBootLen)
            ||(*pulBootLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN < *pulBootLen)
            ||(*pulBootLen + IDIO_LEN + OEM_CA_LEN + IDIO_LEN > BOOTLOAD_SIZE_MAX))
        {
            print_info_with_u32("\r\nBL len err:0x", *pulBootLen);
            return ERRBOOTLEN;
        }

        /* 加上镜像签名、OEM CA和OEM CA签名的长度 */
        *pulBootLen += IDIO_LEN + OEM_CA_LEN + IDIO_LEN;
    }

    return ulRetVal;
}

#ifdef EMMC_DEBUG
UINT32 emmc_normal_mode_init(UINT32 slot, EMMC_GLOBAL_VAR_S *pstGlbVar)
{
    UINT32 ulRetVal;

    /*
        default value is not suitable for emmc, need config .
    */
    EMMC_SYS_CONFIG();
    EMMC_IOS_SYS_CONFIG();
    EMMC_SET_CLK();
    /*
        Config the timer0.
        EMMC use it for timeout check.
    */
    TIMER0_INIT();
    /* Now the timer is active */

    emmc_config_od(FALSE);

    if((ulRetVal= emmc_reset_ip()))
    {
        return ulRetVal;
    }

    emmc_global_variable_init(slot, pstGlbVar, TRUE);

#ifdef EMMC_IDMAC_SUPPORT
    if((ulRetVal = emmc_idmac_init(pstGlbVar)))
    {
        print_info("emmc idmac init failed\r\n");
        return ulRetVal;
    }
#endif

    emmc_interrupt_setup();

    /* Set Data and Response timeout */
    OUTREG32(EMMC_REG_TMOUT, 0x030D40FF);   /* Timeout is 500ms@400K */

    /* Set the card Debounce to allow the CDETECT fluctuations to settle down*/
    OUTREG32(EMMC_REG_DEBNCE, DEFAULT_DEBNCE_VAL);

    emmc_set_fifothresh();

    if((ulRetVal = emmc_enumerate_the_card(slot,pstGlbVar)))
    {
        print_info("emmc enumerate card failed\r\n");
        return ulRetVal;
    }

#ifdef EMMC_DEBUG
    emmc_cardinfo_show(slot,pstGlbVar);
#endif

    if((ulRetVal = emmc_check_transtate(slot,pstGlbVar)))
    {
        return ulRetVal;
    }

    if((ulRetVal = emmc_set_default_clk(slot,pstGlbVar)))
    {
        print_info( (UINT8 *)"set default clk fail\r\n");
        return ulRetVal;
    }

    /* Set Data and Response timeout */
    OUTREG32(EMMC_REG_TMOUT, 0xB71B00FF);   /* Timeout is 500ms@24M */

    return ERRNOERROR;
}
#endif

/*****************************************************************************
* 函 数 名  : emmc_read_bootloader_normal
*
* 功能描述  : 读取bootloader数据
*
* 输入参数  : UINT32 slot     :卡槽
*             UINT32 dst      :数据buffer
*             BOOL bUseHold   :CMD寄存器use_hold_reg的值
* 输出参数  : 无
*
* 返 回 值  : ERRNOERROR    :读取成功
*             ERRENUMERATE  :枚举失败
*             其它          :读取失败
* 其它说明  : 无
*
*****************************************************************************/
UINT32 emmc_read_bootloader_normal(UINT32 slot, UINT32 dst, BOOL bUseHold)
{
    UINT32 ulRetVal = 0;
    UINT32 ulBootAddr = 0;
    UINT32 ulBootLen = 0;
    EMMC_GLOBAL_VAR_S stGlbVar;
    EMMC_GLOBAL_VAR_S *pstGlbVar = &stGlbVar;

    emmc_config_od(FALSE);

    if((ulRetVal= emmc_reset_ip()))
    {
        return ulRetVal;
    }

    emmc_global_variable_init(slot, pstGlbVar, bUseHold);

#ifdef EMMC_IDMAC_SUPPORT
    if((ulRetVal = emmc_idmac_init(pstGlbVar)))
    {
        return ulRetVal;
    }
#endif

    emmc_interrupt_setup();

    /* Set Data and Response timeout */
    OUTREG32(EMMC_REG_TMOUT, 0x030D40FF);   /* Timeout is 300ms@400K */

    /* Set the card Debounce to allow the CDETECT fluctuations to settle down */
    OUTREG32(EMMC_REG_DEBNCE, DEFAULT_DEBNCE_VAL);

    emmc_set_fifothresh();

	ulRetVal = emmc_enumerate_the_card(slot,pstGlbVar);
    if(ulRetVal)
    {
        if(ERRCARDNOTCONN == ulRetVal)
        {
            return ulRetVal;
        }
        else
        {
            print_info("\r\nenum card fail:");
            print_info_with_u32("\r\n    bUseHold=0x", (UINT32)bUseHold);
            return ERRENUMERATE;
        }
    }
#ifdef EMMC_DEBUG
    emmc_cardinfo_show(slot, pstGlbVar);
#endif

    ulRetVal = emmc_check_transtate(slot,pstGlbVar);
    if(ulRetVal)
    {
        return ulRetVal;
    }

    /* Set Data and Response timeout */
    OUTREG32(EMMC_REG_TMOUT, 0xB71B00FF);   /* Timeout is 500ms@24M */

    /* check if boot mode enable */
    if(pstGlbVar->stCardInfo[slot].extcsd_bootop)
    {
        ulRetVal = emmc_read_bootloader_boot_mode(slot, pstGlbVar, dst);
        return ulRetVal;
    }

     /* 将切24M时钟放在boot模式之后，因为boot模式在24M时钟下时序有风险 ;但是挪到后面会造成boot模式读取超时，故与V7保持一致*/
    if((ulRetVal = emmc_set_default_clk(slot,pstGlbVar)))
    {
        return ulRetVal;
    }
    ulRetVal = emmc_get_bootinfo(slot,pstGlbVar,(UINT8 *)dst,&ulBootAddr,&ulBootLen);
    if(ulRetVal)
    {
        return ulRetVal;
    }

    /* set to sector align */
    ulBootLen = (ulBootLen + EMMC_SEC_SIZE - 1)/EMMC_SEC_SIZE*EMMC_SEC_SIZE;

    ulRetVal = emmc_block_read(slot, pstGlbVar, ulBootAddr, (UINT8 *)dst, ulBootLen);
    if(ulRetVal)
    {
        print_info((UINT8 *)"\r\nnomal read bootloader fail");
    }
    else
    {
#ifdef EMMC_DEBUG
        print_info((UINT8 *)"\r\nnomal read bootloader success");
#endif
    }

#ifdef EMMC_DEBUG
    emmc_packet_dump((UINT8 *)dst);
#endif

    return ulRetVal;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
