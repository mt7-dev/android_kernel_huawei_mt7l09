
#include "types.h"
#include "time.h"
#include "boot/boot.h"
#include "bsp_shared_ddr.h"
#include "ddm_phase.h"

#include "product_config.h"

#ifdef CONFIG_DDM

#define PHASE_MAX_NAME_SIZE     256

#define RANGE_CONFLICT(s1,e1,s2,e2) ((((s1) >= (e2))||((s2) >= (e1)))?FALSE:TRUE)
#define ALIGN_SIZE(p, a)  (((p) + ((a) - 1)) & ~((a) - 1))
#define OSAL_LogMsg printf

typedef enum
{
    POOL_DIRECT_SINGLE,                 /*when  pool reach end, stop malloc buffer */
    POOL_DIRECT_INHERIT,                /*when  pool reach end, get buffer from system malloc */
    POOL_RING_CIRCLE                    /*when  pool reach end, get buffer from start of pool again  */
}DDM_POOL_TYPE;

typedef enum
{
    POOL_PARENT_LOAD,                   /*pool controller is inited in fastboot*/
    POOL_PARENT_OS                      /*pool controller is inited in vxworks OS*/
}DDM_POOL_PARENT;


typedef struct _buff_list
{
    unsigned int    bufSize  ;
    struct _buff_list *p_next;
}DDM_BUFF_LIST;

typedef struct
{
   DDM_BUFF_LIST*          pData;       /*point to the start of dump data list*/
   DDM_BUFF_LIST*          poolTail;    /*point to the last pool buff */
   unsigned int            sPool;       /*point to local memory pool start address*/
   unsigned int            ePool;       /*indicate the end address of local pool */ 
   unsigned int            pFree;       /*point to local memory pool free address*/
   unsigned int            nType ;      /*memory pool usage type: circle or single mode DDM_POOL_TYPE*/
   unsigned int            nParent ;    /*memory pool usage type: circle or single mode DDM_POOL_TYPE*/

}DDM_POOL_CTRL;

typedef struct 
{
    DDM_BUFF_LIST   dBuff;            /*struct buffer header    */
    unsigned int    slice;            /*phase slice ,time information */
    unsigned int    taskId;           /*phase task id ,which task call this phase record function*/  
    unsigned int    value;            /*custom information ,usr-defined value*/
    char *          phName;           /*string contain this phase describ information, */
}DDM_PHASE_DATA;

#define PHASE_BOOT_POOL_ADDR	(SHM_DDM_LOAD_ADDR + sizeof(DDM_POOL_CTRL))
#define PHASE_BOOT_POOL_SIZE	(SHM_DDM_LOAD_SIZE - sizeof(DDM_POOL_CTRL))
static DDM_POOL_CTRL*  p_score_boot_ctrl = (DDM_POOL_CTRL *)(SHM_DDM_LOAD_ADDR);
static unsigned char *  p_score_boot_pool = (unsigned char *)(PHASE_BOOT_POOL_ADDR);
static char* ddm_phase_buf_get(unsigned int reqSize, DDM_POOL_CTRL* pPoolCtrl)
{
    char *RetBuf = NULL ;
    DDM_POOL_CTRL*  pCtrl = pPoolCtrl;
    unsigned int bufEndAddr = 0;
    
    bufEndAddr = pCtrl->pFree + reqSize;
    switch (pCtrl->nType)
    {
        case POOL_DIRECT_SINGLE:
        {
            if (bufEndAddr <= pCtrl->ePool)
            {
                RetBuf = (char*)pCtrl->pFree;
                pCtrl->pFree = bufEndAddr;
            }
            else
            {
                /*local pool deplenish, return NULL*/
                RetBuf = NULL;
            }
        }
        break;
        
       default:
        break;
    }
        
    return RetBuf;
}

char* strncpy(char *dest, const char *src, unsigned int count)
{
	char*tmp = dest;

	while(count-- && ((*dest++ = *src++)!= '\0'))
	/*do nothing*/;

	return tmp;
}



static int ddm_phase_score(DDM_POOL_CTRL* pPhaseCtrl ,const char* phaseName,unsigned int param)
{
    unsigned int strSize;
    unsigned int strLength;
    unsigned int reqSize;
    DDM_BUFF_LIST * pBuffNew = NULL ;
    DDM_BUFF_LIST * pBuffLast = NULL;
    DDM_PHASE_DATA* pPhaseData = NULL;

    if(NULL == pPhaseCtrl)
    {
        return DDM_ERROR;
    }
    /*malloc buffer first*/
    
    strSize = strlen(phaseName) + 1 ;
    strSize = (strSize <= PHASE_MAX_NAME_SIZE)?strSize:PHASE_MAX_NAME_SIZE;
    strLength = strSize - 1;
    reqSize = sizeof(DDM_PHASE_DATA) + strSize;

    reqSize = ALIGN_SIZE(reqSize,4);
        
    pBuffNew = (DDM_BUFF_LIST*)ddm_phase_buf_get(reqSize,pPhaseCtrl);
    if(NULL == pBuffNew)
    {
        return DDM_ERROR;
    }

    pBuffNew->bufSize = reqSize;
    
    /*如果链表为空，初始化当前数据指针*/
    if(NULL == pPhaseCtrl->pData)
    {
        pPhaseCtrl->pData = pBuffNew;
        pBuffNew->p_next = NULL;
    }
    else
    {
        pBuffLast  = pPhaseCtrl->poolTail;
        /*确保指向最后一个*/
        while(NULL != pBuffLast->p_next)
        {
            pBuffLast = pBuffLast->p_next;
        }
        /*add new node to the end of list*/
        pBuffLast->p_next = pBuffNew;
        pBuffNew->p_next = NULL;
    }

    pPhaseCtrl->poolTail = pBuffNew;

    pPhaseData = (DDM_PHASE_DATA*)pBuffNew;
        
    /*安全拷贝,使文本长度不超过SCORE_MAX_NAME_SIZE*/
    pPhaseData->phName = (char *)((unsigned int)pPhaseData + sizeof(DDM_PHASE_DATA));
    strncpy(&pPhaseData->phName[0], phaseName,strLength);
    pPhaseData->phName[strLength] = '\0';

    pPhaseData->taskId = 0;
    pPhaseData->slice = get_elapse_ms();/*lint !e732*/
    pPhaseData->value = param;

   return DDM_OK;
}



static int ddm_phase_init(DDM_POOL_CTRL*   pPhaseCtrl, void* pScorePool,unsigned int pSize ,DDM_POOL_TYPE type ,DDM_POOL_PARENT parent)
{
    pPhaseCtrl->sPool = (unsigned int)pScorePool;
    pPhaseCtrl->ePool = (unsigned int)pScorePool + pSize ;
    pPhaseCtrl->pFree = (unsigned int)pScorePool;
    pPhaseCtrl->pData = NULL;
    pPhaseCtrl->nType = type;
    pPhaseCtrl->nParent = parent;
    pPhaseCtrl->poolTail = NULL;

    return DDM_OK;
}


int ddm_phase_boot_init(void)
{
   DDM_POOL_CTRL* pCtrl = p_score_boot_ctrl;
   
   memset(p_score_boot_ctrl,0,sizeof(DDM_POOL_CTRL));
   memset(p_score_boot_pool,0,SHM_DDM_LOAD_SIZE - sizeof(DDM_POOL_CTRL));
    
   return ddm_phase_init(pCtrl, p_score_boot_pool, PHASE_BOOT_POOL_SIZE, POOL_DIRECT_SINGLE,POOL_PARENT_LOAD);
}

int ddm_phase_boot_score(const char * phaseName, unsigned int param)
{
    DDM_POOL_CTRL* pCtrl = p_score_boot_ctrl;

    if((unsigned int)p_score_boot_pool != pCtrl->sPool)
    {
        if(DDM_OK !=  ddm_phase_boot_init())
        {
            return DDM_ERROR;
        }
    }
    
    return ddm_phase_score(pCtrl, phaseName, param);
}

#else

int ddm_phase_boot_init(void)
{
    return 0;

}

int ddm_phase_boot_score(const char * phaseName, unsigned int param)
{
    return 0;
}

#endif

