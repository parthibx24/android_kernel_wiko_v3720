

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
//#include <asm/atomic.h>
#include <linux/slab.h>


#include "kd_camera_hw.h"
#include "kd_imgsensor.h" 
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "cam_cal.h"
#include "cam_cal_define.h"

#include "s5k4h8mipi_Sensor.h"

//#include <linux/xlog.h>

int flag_otp=0x00;
int VCMID=0x00;
int DriverICID=0x00;
int RGr_ratio=0x00;
int BGr_ratio=0x00;
int GbGr_ratio=0x00;
int VCM_start=0x00;
int VCM_end=0x00;


//struct Darling_S5K4H8_otp_struct *S5K4H8_OTP =NULL;
u8 otp_4h8_id_data[10];
u16 otp_4h8_af_data[2];
#define RGr_ratio_Typical  300	//modify by v3710 601
#define BGr_ratio_Typical  250  //modify by v3710 543
#define GbGr_ratio_Typical  512

#define LOG_TAG  "S5K4H8:"
extern  void S5K4H8_wordwrite_cmos_sensor(u16 addr, kal_uint16 para);
extern  void S5K4H8_bytewrite_cmos_sensor(u16 addr, kal_uint16 para);
extern kal_uint16 S5K4H8_byteread_cmos_sensor(u16 addr);

//#define SENSORDB_S5K4H8_OTP(fmt,arg...) xlog_printk(ANDROID_LOG_DEBUG , "[S5K3H7_OTP]", fmt, ##arg)                             //printk(LOG_TAG "%s: " fmt "\n", __FUNCTION__ ,##arg)


#define SENSORDB_S5K4H8_OTP(fmt,arg...)  pr_debug(LOG_TAG "%s: " fmt "\n", __FUNCTION__ ,##arg)

static bool start_read_otp_ofilm(void)
{
    BYTE val = 0;
    int i;
    //S5K4H8_wordwrite_cmos_sensor(0xFCFC, 0xD000);
    S5K4H8_bytewrite_cmos_sensor(0x0A02,0x0f); // 0x0f Select the page to write by writing to 0xD0000A02 0x01~0x0C
    S5K4H8_bytewrite_cmos_sensor(0x0A00, 0x01); //Enter read mode by writing 01h    to 0xD0000A00
	mdelay(15);

   val = S5K4H8_byteread_cmos_sensor(0x0A04);
	SENSORDB_S5K4H8_OTP(" read 0x0A04 = 0x %x\n,",val);
	SENSORDB_S5K4H8_OTP(" read val = 0x %x\n,",val);

	 if(val == 0x40){
	 	
		return 1;
	 }

	 else{

 		val = S5K4H8_byteread_cmos_sensor(0x0A24);
        SENSORDB_S5K4H8_OTP(" read 0x0A24 = 0x %x\n,",val);

		SENSORDB_S5K4H8_OTP(" read val = 0x %x\n,",val);
	 if(val == 0x01){
	 		
			return 1;
		 }
	}
		
    return 0;
}



/*************************************************************************************************
* Function : stop_read_otp_sunny
* Description : after read otp_sunny , stop and reset otp_sunny block setting
**************************************************************************************************/
 void stop_read_otp_ofilm(void)
{
    S5K4H8_bytewrite_cmos_sensor(0x0A00, 0x00); //Reset the NVM interface bywriting 00h to 0xD0000A00
}


void Ofilm_S5K4H8_read_OTP(void)
{

		int i = 0;
		unsigned char val = 0;
		int a26=0,a27=0,a28=0,a29=0,a30=0,a31=0,a32=0,a33=0,a34=0,a35=0,a36=0,a37=0,a38=0,a39=0,a=0;
		SENSORDB_S5K4H8_OTP(" Darling_S5K4H8_read_OTP \n");
		S5K4H8_bytewrite_cmos_sensor(0x0A02,0x0f);
		S5K4H8_bytewrite_cmos_sensor(0x0A00,0x01);
        mdelay(15);
		
		     val=S5K4H8_byteread_cmos_sensor(0x0A04);//flag of info and awb


		SENSORDB_S5K4H8_OTP(" read 0x0A04 = 0x %x\n,",val);
		
	if(val == 0x40)
	{
		flag_otp = 0x01;
		RGr_ratio =(S5K4H8_byteread_cmos_sensor(0x0A07)<<2)|((S5K4H8_byteread_cmos_sensor(0x0A0A)>>6)&0x03);
		BGr_ratio =(S5K4H8_byteread_cmos_sensor(0x0A08)<<2)|((S5K4H8_byteread_cmos_sensor(0x0A0A)>>4)&0x03);
		GbGr_ratio =(S5K4H8_byteread_cmos_sensor(0x0A09)<<2)|((S5K4H8_byteread_cmos_sensor(0x0A0A)>>2)&0x03);
		SENSORDB_S5K4H8_OTP("HYNIX_CAM_CAL RGr_ratio= 0x%x, BGr_ratio = 0x%x, GbGr_ratio = 0x%0x\n",RGr_ratio,BGr_ratio,GbGr_ratio);
	}
	else if(val == 0xD0)
	{
		flag_otp = 0x01;
		RGr_ratio =(S5K4H8_byteread_cmos_sensor(0x0A0E)<<2)|((S5K4H8_byteread_cmos_sensor(0x0A10)>>6)&0x03);
		BGr_ratio =(S5K4H8_byteread_cmos_sensor(0x0A0F)<<2)|((S5K4H8_byteread_cmos_sensor(0x0A10)>>4)&0x03);
		GbGr_ratio =(S5K4H8_byteread_cmos_sensor(0x0A10)<<2)|((S5K4H8_byteread_cmos_sensor(0x0A10)>>2)&0x03);
		SENSORDB_S5K4H8_OTP("HYNIX_CAM_CAL RGr_ratio= 0x%x, BGr_ratio = 0x%x, GbGr_ratio = 0x%0x\n",RGr_ratio,BGr_ratio,GbGr_ratio);
	}
	else
		{
		flag_otp=0x00;	
	}
	
	
	val=S5K4H8_byteread_cmos_sensor(0x0A13);//falg of VCM
	SENSORDB_S5K4H8_OTP(" read VCM flag 0x0A14 = 0x %x\n,",val);
	if(val == 0x40)
	{
		flag_otp += 0x04;
      		otp_4h8_af_data[0]=((S5K4H8_byteread_cmos_sensor(0x0A14) << 2) |((S5K4H8_byteread_cmos_sensor(0x0A16)>>6)&0x03)) ;
	  otp_4h8_af_data[1]=((S5K4H8_byteread_cmos_sensor(0x0A15) << 2) |((S5K4H8_byteread_cmos_sensor(0x0A16)>>6)&0x03));
		SENSORDB_S5K4H8_OTP("S5K4H8 S5K4H8_byteread_cmos_sensorotp_4h8_af_data = 0x%x\n", otp_4h8_af_data[0]);
		SENSORDB_S5K4H8_OTP("S5K4H8 S5K4H8_byteread_cmos_sensor otp_4h8_af_data[1] = 0x%x\n",otp_4h8_af_data[1]);	
	}
	else if(val == 0xD0)
		{
		flag_otp += 0x04;
      		otp_4h8_af_data[0]=((S5K4H8_byteread_cmos_sensor(0x0A18) << 2) |((S5K4H8_byteread_cmos_sensor(0x0A1a)>>4)&0x03)) ;
	  otp_4h8_af_data[1]=((S5K4H8_byteread_cmos_sensor(0x0A19) << 2) |((S5K4H8_byteread_cmos_sensor(0x0A1a)>>4)&0x03));
		SENSORDB_S5K4H8_OTP("S5K4H8 S5K4H8_byteread_cmos_sensorotp_4h8_af_data = 0x%x\n", otp_4h8_af_data[0]);
		SENSORDB_S5K4H8_OTP("S5K4H8 S5K4H8_byteread_cmos_sensor otp_4h8_af_data[1] = 0x%x\n",otp_4h8_af_data[1]);	
	}
	else if(val == 0x00)
	{
		flag_otp += 0x00;
		VCM_start = 0x00;
		VCM_end = 0x00;
	}
	
	stop_read_otp_ofilm();

}
static int RG_RATIO_TYPICAL_VALUE = 0;
static int BG_RATIO_TYPICAL_VALUE = 0;

void Ofilm_S5K4H8_apply_OTP(void){

		SENSORDB_S5K4H8_OTP(" Darling_S5K4H8_apply_OTP flag_otp = %d \n",flag_otp);
	
	if(((flag_otp)&0x03) != 0x01){ 

		SENSORDB_S5K4H8_OTP(" Darling_S5K4H8_apply_OTP  err:  \n");
		return;

	}

			int R_to_G = 0,B_to_G = 0;
			int R_Gain = 0,B_Gain = 0,G_Gain = 0;
			int G_gain_R = 0,G_gain_B = 0;
			int RG_RATIO_TYPICAL_VALUE=0,BG_RATIO_TYPICAL_VALUE=0;
			
			R_to_G = RGr_ratio;
			B_to_G = BGr_ratio;
			RG_RATIO_TYPICAL_VALUE = RGr_ratio_Typical;
			BG_RATIO_TYPICAL_VALUE = BGr_ratio_Typical;
			 SENSORDB_S5K4H8_OTP("R_to_G:0x%0x,B_to_G:0x%0x", R_to_G, B_to_G);
			SENSORDB_S5K4H8_OTP("R_to_G_golden:0x%0x,B_to_G_golden:0x%0x", RG_RATIO_TYPICAL_VALUE, BG_RATIO_TYPICAL_VALUE);
			if(B_to_G < BG_RATIO_TYPICAL_VALUE)
			{
				if(R_to_G < RG_RATIO_TYPICAL_VALUE)
				{
					G_Gain = 0x100;
					B_Gain = 0x100 * BG_RATIO_TYPICAL_VALUE / B_to_G;
					R_Gain = 0x100 * RG_RATIO_TYPICAL_VALUE / R_to_G;
				}
				else
				{
					R_Gain = 0x100;
					G_Gain = 0x100 * R_to_G / RG_RATIO_TYPICAL_VALUE;
					B_Gain = G_Gain * BG_RATIO_TYPICAL_VALUE / B_to_G;			
				}
			}
			else
			{
				if(R_to_G < RG_RATIO_TYPICAL_VALUE)
				{
					B_Gain = 0x100;
					G_Gain = 0x100 * B_to_G / BG_RATIO_TYPICAL_VALUE;
					R_Gain = G_Gain * RG_RATIO_TYPICAL_VALUE / R_to_G;
				}
				else
				{
					G_gain_B = 0x100*B_to_G / BG_RATIO_TYPICAL_VALUE;
					G_gain_R = 0x100*R_to_G / RG_RATIO_TYPICAL_VALUE;
							
					if(G_gain_B > G_gain_R)
					{
						B_Gain = 0x100;
						G_Gain = G_gain_B;
						R_Gain = G_Gain * RG_RATIO_TYPICAL_VALUE / R_to_G;
					}
					else
					{
						R_Gain = 0x100;
						G_Gain = G_gain_R;
						B_Gain = G_Gain * BG_RATIO_TYPICAL_VALUE / B_to_G;
					}		 
				}	
			}		
		  /* update digital gain */
		  SENSORDB_S5K4H8_OTP("R_gain:0x%0x,G_gain:0x%0x, B_gain:0x%0x,", R_Gain, G_Gain, B_Gain);
	S5K4H8_bytewrite_cmos_sensor(0x3058,0x01);	//1:enable awb otp , add by v3702

	if(G_Gain>0x100)
	{
		S5K4H8_bytewrite_cmos_sensor(0x020E,G_Gain>>8);
		S5K4H8_bytewrite_cmos_sensor(0x020F,G_Gain&0xff);
	}
	if(R_Gain>0x100)
	{
		S5K4H8_bytewrite_cmos_sensor(0x0210,R_Gain>>8);
		S5K4H8_bytewrite_cmos_sensor(0x0211,R_Gain&0xff);
	}
	if(B_Gain>0x100)
	{
		S5K4H8_bytewrite_cmos_sensor(0x0212,B_Gain>>8);
		S5K4H8_bytewrite_cmos_sensor(0x0213,B_Gain&0xff);
	}
	if(G_Gain>0x100)
	{
		S5K4H8_bytewrite_cmos_sensor(0x0214,G_Gain>>8);
		S5K4H8_bytewrite_cmos_sensor(0x0215,G_Gain&0xff);
		
	}

	SENSORDB_S5K4H8_OTP(" update otp ok \n,");
	
}



	/*************************************************************************************************
* Function : otp_ofilm_update_wb()
* Description : update otp_sunny data from otp_sunny , it otp_sunny data is valid,
it include get ID and WB update function
* Return : [bool] 0 : update fail
1 : update success
**************************************************************************************************/
void otp_ofilm_update_wb(void)
{
    BYTE zone = 0x01;
    BYTE FLG = 0x00;
    BYTE MID = 0x00;
	//BYTE LENS_ID= 0x00,VCM_ID= 0x00;
    int i;
    
    /*  if(!start_read_otp_ofilm()){

		  stop_read_otp_ofilm();
	  
           SENSORDB_S5K4H8_OTP(" No otp_ofilm Data or otp_ofilm data is invalid!!");
		//   return 0;
      	}

	  stop_read_otp_ofilm();*/
    SENSORDB_S5K4H8_OTP(" statrt read otp  \n");

    Ofilm_S5K4H8_read_OTP();
	SENSORDB_S5K4H8_OTP(" statrt write otp  \n");
	Ofilm_S5K4H8_apply_OTP();
	SENSORDB_S5K4H8_OTP("  write otp end \n");
  //  return 1;
}

static DEFINE_SPINLOCK(g_CAM_CALLock); // for SMP
#define CAM_CAL_I2C_BUSNUM 0
#define S5K4H8_DEVICE_ID 0x20
#define CAM_CAL_DEV_MAJOR_NUMBER 226

/*******************************************************************************
*
********************************************************************************/
#define CAM_CAL_ICS_REVISION 1 //seanlin111208
/*******************************************************************************
*
********************************************************************************/
#define CAM_CAL_DRVNAME "S5K4H8_CAM_CAL_DRV"
//#define CAM_CAL_DRVNAME "CAM_CAL_DRV"
#define CAM_CAL_I2C_GROUP_ID 0
/*******************************************************************************
*
********************************************************************************/
/* fix warning MSG 
static unsigned short g_pu2Normal_i2c[] = {S5K4H8_DEVICE_ID , I2C_CLIENT_END};
static unsigned short g_u2Ignore = I2C_CLIENT_END;
static struct i2c_client_address_data g_stCAM_CAL_Addr_data = {
    .normal_i2c = g_pu2Normal_i2c,
    .probe = &g_u2Ignore,
    .ignore = &g_u2Ignore
}; */

static struct i2c_board_info __initdata kd_cam_cal_dev={ I2C_BOARD_INFO(CAM_CAL_DRVNAME, 0x20>>1)}; //make dummy_eeprom co-exist

//81 is used for V4L driver
static dev_t g_CAM_CALdevno = MKDEV(CAM_CAL_DEV_MAJOR_NUMBER,0);
static struct cdev * g_pCAM_CAL_CharDrv = NULL;
//static spinlock_t g_CAM_CALLock;
static struct class *CAM_CAL_class = NULL;
static atomic_t g_CAM_CALatomic;
/*******************************************************************************
*
********************************************************************************/
//UINT32 S5k4H8_CheckID = 0x884000dd;
u8 S5k4H8_CheckID[]= {0x10,0xdd,0x00,0x40,0x88};

static int selective_read_region(u32 offset, BYTE* data,u16 i2c_id,u32 size)
{    	
    printk("[s5k4h8_CAM_CAL] selective_read_region offset =%d size %d data read = %d\n", offset,size, *data);

if(size ==2 ){
	if(offset == 7){
		memcpy((void *)data,(void *)&otp_4h8_af_data[0],size);
	}
	
	else if(offset == 9){		 
		memcpy((void *)data,(void *)&otp_4h8_af_data[1],size);

	}
}

if(size == 4){
		//memcpy((void *)data,(void *)&otp_4h8_id_data[offset],size);
		memcpy((void *)data,(void *)&S5k4H8_CheckID[1],size);

	}
	printk("+ls.test[s5k4h8_CAM_CAL]  data1 = %d\n",*(UINT32 *)data);
//	memcpy((void *)data,(void *)&otp_test1,size);
//	printk("+ls.test[s5k4h8_CAM_CAL]  data2 = %d\n",*(UINT32 *)data);


}



/*******************************************************************************
*
********************************************************************************/
#define NEW_UNLOCK_IOCTL
#ifndef NEW_UNLOCK_IOCTL
static int CAM_CAL_Ioctl(struct inode * a_pstInode,
struct file * a_pstFile,
unsigned int a_u4Command,
unsigned long a_u4Param)
#else 
static long CAM_CAL_Ioctl(
    struct file *file, 
    unsigned int a_u4Command, 
    unsigned long a_u4Param
)
#endif
{
    int i4RetValue = 0;
    u8 * pBuff = NULL;
    u8 * pWorkingBuff = NULL;
    stCAM_CAL_INFO_STRUCT *ptempbuf;

#ifdef CAM_CALGETDLT_DEBUG
    struct timeval ktv1, ktv2;
    unsigned long TimeIntervalUS;
#endif

    if(_IOC_NONE == _IOC_DIR(a_u4Command))
    {
    }
    else
    {
        pBuff = (u8 *)kmalloc(sizeof(stCAM_CAL_INFO_STRUCT),GFP_KERNEL);

        if(NULL == pBuff)
        {
            SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] ioctl allocate mem failed\n");
            return -ENOMEM;
        }

        if(_IOC_WRITE & _IOC_DIR(a_u4Command))
        {
            if(copy_from_user((u8 *) pBuff , (u8 *) a_u4Param, sizeof(stCAM_CAL_INFO_STRUCT)))
            {    //get input structure address
                kfree(pBuff);
                SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] ioctl copy from user failed\n");
                return -EFAULT;
            }
        }
    }

    ptempbuf = (stCAM_CAL_INFO_STRUCT *)pBuff;
    pWorkingBuff = (u8*)kmalloc(ptempbuf->u4Length,GFP_KERNEL); 
    if(NULL == pWorkingBuff)
    {
        kfree(pBuff);
        SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] ioctl allocate mem failed\n");
        return -ENOMEM;
    }
//fix warning MSG     SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] init Working buffer address 0x%x  command is 0x%08x\n", pWorkingBuff, a_u4Command);

 
    if(copy_from_user((u8*)pWorkingBuff ,  (u8*)ptempbuf->pu1Params, ptempbuf->u4Length))
    {
        kfree(pBuff);
        kfree(pWorkingBuff);
        SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] ioctl copy from user failed\n");
        return -EFAULT;
    } 
    
    switch(a_u4Command)
    {


		 case CAM_CALIOC_S_WRITE:
    SENSORDB_S5K4H8_OTP("[SENSORDB_S5K4H8_OTP] CAM_CALIOC_S_WRITE \n");        
#ifdef CAM_CALGETDLT_DEBUG
            do_gettimeofday(&ktv1);
#endif
            i4RetValue = 0;
           // i4RetValue=iWriteData((u16)ptempbuf->u4Offset, ptempbuf->u4Length, pu1Params);
#ifdef CAM_CALGETDLT_DEBUG
            do_gettimeofday(&ktv2);
            if(ktv2.tv_sec > ktv1.tv_sec)
            {
                TimeIntervalUS = ktv1.tv_usec + 1000000 - ktv2.tv_usec;
            }
            else
            {
                TimeIntervalUS = ktv2.tv_usec - ktv1.tv_usec;
            }
#endif
            break;

	
      
        case CAM_CALIOC_G_READ:
            SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] Read CMD \n");
#ifdef CAM_CALGETDLT_DEBUG            
            do_gettimeofday(&ktv1);
#endif 
            SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] offset %d \n", ptempbuf->u4Offset);
            SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] length %d \n", ptempbuf->u4Length);
          // SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] Before read Working buffer address 0x%x \n", pWorkingBuff);


if(ptempbuf->u4Length == 2){

       i4RetValue = selective_read_region(ptempbuf->u4Offset, pWorkingBuff, 0x20, ptempbuf->u4Length);

}else if(ptempbuf->u4Length == 4){

	   i4RetValue = selective_read_region(ptempbuf->u4Offset, pWorkingBuff, 0x20, ptempbuf->u4Length);
 }        
		
          // SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] After read Working buffer address 0x%x \n", pWorkingBuff);


#ifdef CAM_CALGETDLT_DEBUG
            do_gettimeofday(&ktv2);
            if(ktv2.tv_sec > ktv1.tv_sec)
            {
                TimeIntervalUS = ktv1.tv_usec + 1000000 - ktv2.tv_usec;
            }
            else
            {
                TimeIntervalUS = ktv2.tv_usec - ktv1.tv_usec;
            }
            printk("Read data %d bytes take %lu us\n",ptempbuf->u4Length, TimeIntervalUS);
#endif            

            break;
        default :
      	     SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] No CMD \n");
            i4RetValue = -EPERM;
        break;
    }

    if(_IOC_READ & _IOC_DIR(a_u4Command))
    {
        //copy data to user space buffer, keep other input paremeter unchange.
        SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] to user length %d \n", ptempbuf->u4Length);
		
  

  
		if(copy_to_user((u8 __user *) ptempbuf->pu1Params , (u8 *)pWorkingBuff , ptempbuf->u4Length))
        {
            kfree(pBuff);
            kfree(pWorkingBuff);
            SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] ioctl copy to user failed\n");
            return -EFAULT;
        }
    }

    kfree(pBuff);
    kfree(pWorkingBuff);
    return i4RetValue;
}


static u32 g_u4Opened = 0;
//#define
//Main jobs:
// 1.check for device-specified errors, device not ready.
// 2.Initialize the device if it is opened for the first time.
static int CAM_CAL_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    SENSORDB_S5K4H8_OTP("[s5k4h8 CAM_CAL] CAM_CAL_Open\n");
    spin_lock(&g_CAM_CALLock);
    if(g_u4Opened)
    {
        spin_unlock(&g_CAM_CALLock);
        return -EBUSY;
    }
    else
    {
        g_u4Opened = 1;
        atomic_set(&g_CAM_CALatomic,0);
    }
    spin_unlock(&g_CAM_CALLock);

//#if defined(MT6572)
	// do nothing
//#else
    //if(TRUE != hwPowerOn(MT65XX_POWER_LDO_VCAMA, VOL_2800, "S24CS64A"))
    //{
    //    SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] Fail to enable analog gain\n");
    //    return -EIO;
    //}
//#endif	

    return 0;
}

//Main jobs:
// 1.Deallocate anything that "open" allocated in private_data.
// 2.Shut down the device on last close.
// 3.Only called once on last time.
// Q1 : Try release multiple times.
static int CAM_CAL_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
    spin_lock(&g_CAM_CALLock);

    g_u4Opened = 0;

    atomic_set(&g_CAM_CALatomic,0);

    spin_unlock(&g_CAM_CALLock);

    return 0;
}

static const struct file_operations g_stCAM_CAL_fops =
{
    .owner = THIS_MODULE,
    .open = CAM_CAL_Open,
    .release = CAM_CAL_Release,
    //.ioctl = CAM_CAL_Ioctl
    .unlocked_ioctl = CAM_CAL_Ioctl
};

#define CAM_CAL_DYNAMIC_ALLOCATE_DEVNO 1
inline static int RegisterCAM_CALCharDrv(void)
{
    struct device* CAM_CAL_device = NULL;

#if CAM_CAL_DYNAMIC_ALLOCATE_DEVNO
    if( alloc_chrdev_region(&g_CAM_CALdevno, 0, 1,CAM_CAL_DRVNAME) )
    {
        SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] Allocate device no failed\n");

        return -EAGAIN;
    }
#else
    if( register_chrdev_region(  g_CAM_CALdevno , 1 , CAM_CAL_DRVNAME) )
    {
        SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] Register device no failed\n");

        return -EAGAIN;
    }
#endif

    //Allocate driver
    g_pCAM_CAL_CharDrv = cdev_alloc();

    if(NULL == g_pCAM_CAL_CharDrv)
    {
        unregister_chrdev_region(g_CAM_CALdevno, 1);

        SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //Attatch file operation.
    cdev_init(g_pCAM_CAL_CharDrv, &g_stCAM_CAL_fops);

    g_pCAM_CAL_CharDrv->owner = THIS_MODULE;

    //Add to system
    if(cdev_add(g_pCAM_CAL_CharDrv, g_CAM_CALdevno, 1))
    {
        SENSORDB_S5K4H8_OTP("[S5K4H8_CAM_CAL] Attatch file operation failed\n");

        unregister_chrdev_region(g_CAM_CALdevno, 1);

        return -EAGAIN;
    }
 
    CAM_CAL_class = class_create(THIS_MODULE, "S5K4H8_CAM_CALdrv");
    if (IS_ERR(CAM_CAL_class)) {
        int ret = PTR_ERR(CAM_CAL_class);
        SENSORDB_S5K4H8_OTP("Unable to create class, err = %d\n", ret);
        return ret;
    }
	
    CAM_CAL_device = device_create(CAM_CAL_class, NULL, g_CAM_CALdevno, NULL, CAM_CAL_DRVNAME);

    return 0;
}

inline static void UnregisterCAM_CALCharDrv(void)
{
    //Release char driver
    cdev_del(g_pCAM_CAL_CharDrv);

    unregister_chrdev_region(g_CAM_CALdevno, 1);

    device_destroy(CAM_CAL_class, g_CAM_CALdevno);
    class_destroy(CAM_CAL_class);
}

static int CAM_CAL_probe(struct platform_device *pdev)
{

    return 0;//i2c_add_driver(&CAM_CAL_i2c_driver);
}

static int CAM_CAL_remove(struct platform_device *pdev)
{
    //i2c_del_driver(&CAM_CAL_i2c_driver);
    return 0;
}

// platform structure
static struct platform_driver g_stCAM_CAL_Driver = {
    .probe		= CAM_CAL_probe,
    .remove	= CAM_CAL_remove,
    .driver		= {
        .name	= CAM_CAL_DRVNAME,
        .owner	= THIS_MODULE,
    }
};


static struct platform_device g_stCAM_CAL_Device = {
    .name = CAM_CAL_DRVNAME,
    .id = 0,
    .dev = {
    }
};

static int __init CAM_CAL_i2C_init(void)
{

   // i2c_register_board_info(CAM_CAL_I2C_BUSNUM, &kd_cam_cal_dev, 1);


	 int i4RetValue = 0;
    SENSORDB_S5K4H8_OTP("[s5k4h8_CAM_CAL]\n");
   //Register char driver
	i4RetValue = RegisterCAM_CALCharDrv();
    if(i4RetValue){
 	   SENSORDB_S5K4H8_OTP(" [s5k4h8_CAM_CAL] register char device failed!\n");
	   return i4RetValue;
	}
	SENSORDB_S5K4H8_OTP(" [s5k4h8_CAM_CAL] Attached!! \n");

   
    if(platform_driver_register(&g_stCAM_CAL_Driver)){
        printk("failed to register CAM_CAL driver\n");
        return -ENODEV;
    }
    if (platform_device_register(&g_stCAM_CAL_Device))
    {
        printk("failed to register CAM_CAL driver\n");
        return -ENODEV;
    }	
SENSORDB_S5K4H8_OTP(" s5k4h8_CAM_CAL  Attached Pass !! \n");
    return 0;
}

static void __exit CAM_CAL_i2C_exit(void)
{
	platform_driver_unregister(&g_stCAM_CAL_Driver);
}

module_init(CAM_CAL_i2C_init);
module_exit(CAM_CAL_i2C_exit);




















	

