#!/bin/bash

# Top directory
SRCTOP="$(pwd)/../../.."
echo ${SRCTOP}

# Specify every platform
LINUXROOT="$SRCTOP/ANDROID/android-2.6.35"
echo ${LINUXROOT}

PRODUCT_VER=GU_V7R1

# Include directory is defined here
INCLUDE_FLAG="-I$LINUXROOT/include/linux"
echo ${INCLUDE_FLAG}

# Display verbose
DISVERBOSE=0

export SRCTOP INCLUDE_FLAG LINUXROOT DISVERBOSE PLAT_FORM PLAT_FORM_FLAG PLAT_FORM_PRODUCT PRODUCT_VER

function build_HI6920ES_STICK(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920ES"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920ES"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920ES_STICK"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log        
}

function build_HI6920ES_PAD(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920ES"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920ES"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920ES_PAD"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log        
}

function build_HI6920ES_UDP(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920ES"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920ES"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920ES_UDP"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log        
}

function build_HI6920ES_UDPE5(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920ES"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920ES"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920ES_UDPE5"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log        
}

function build_HI6920ES_E5776S(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920ES"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920ES"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920ES_E5776S"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log        
}

function build_GUL_HI6920CS_P500(){
        PLAT_FORM="GUL_FPGA"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_FPGA_P500 VERSION_TYPE=CHIP_BB_6920CS"
        PLAT_FORM_FLAG="-DBOARD_FPGA_P500 -DCHIP_BB_6920CS"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920CS_P500"

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log       
}

function build_GUL_HI6920CS_SFT(){
        PLAT_FORM="HI6920CS_SFT"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_SFT VERSION_TYPE=CHIP_BB_6920CS"
        PLAT_FORM_FLAG="-DBOARD_SFT -DCHIP_BB_6920CS"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920CS_SFT"

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log       
}

function build_GUL_P500_FPGA(){
        PLAT_FORM="GUL_FPGA"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_FPGA_P500 VERSION_TYPE=CHIP_BB_6920ES"
        PLAT_FORM_FLAG="-DBOARD_FPGA_P500 -DCHIP_BB_6920ES"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="GUL_FPGA"

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log       
}


function build_HI6920CS_STICK(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920CS"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920CS"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920CS_STICK"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log
}

function build_HI6920CS_PAD(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920CS"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920CS"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920CS_PAD"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log
}

function build_HI6920CS_UDP(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920CS"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920CS"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920CS_UDP"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log
}

function build_HI6920CS_E5776S(){
        PLAT_FORM="GUL_ASIC"
		KERNEL_FLAGS="BOARD_TYPE=BOARD_ASIC VERSION_TYPE=CHIP_BB_6920CS"
        PLAT_FORM_FLAG="-DBOARD_ASIC -DCHIP_BB_6920CS"
        echo ${PLAT_FORM}
        PLAT_FORM_PRODUCT="HI6920CS_E5776S"
        echo ${PLAT_FORM_PRODUCT}

        cd $SRCTOP/PS_CODE/COMM_CODE/MakeFile_LTE

        make clean
        make -j24 ${KERNEL_FLAGS} 2>&1 | tee > make.log
}


case "$1" in
        "HI6920ES_STICK" ) shift ; build_HI6920ES_STICK $@ ;;
        "HI6920ES_PAD" ) shift ; build_HI6920ES_PAD $@ ;;
		"HI6920ES_UDP" ) shift ; build_HI6920ES_UDP $@ ;;
		"HI6920ES_UDPE5" ) shift ; build_HI6920ES_UDPE5 $@ ;;
		"HI6920ES_E5776S" ) shift ; build_HI6920ES_E5776S $@ ;;
		"HI6920CS_P500" ) shift ; build_GUL_HI6920CS_P500 $@ ;;
		"HI6920CS_SFT" ) shift ; build_GUL_HI6920CS_SFT $@ ;;
		"P500_FPGA" ) shift ; build_GUL_P500_FPGA $@ ;;
		"HI6920CS_STICK" ) shift ; build_HI6920CS_STICK $@ ;;
		"HI6920CS_PAD" ) shift ; build_HI6920CS_PAD $@ ;;		
		"HI6920CS_UDP" ) shift ; build_HI6920CS_UDP $@ ;;
		"HI6920CS_E5776S" ) shift ; build_HI6920CS_E5776S $@ ;;


        * ) echo "Usage: ./Make_LPS_STICK_ACPU.sh HI6920ES_STICK";
            echo "       ./Make_LPS_STICK_ACPU.sh HI6920ES_PAD";
	    echo "       ./Make_LPS_STICK_ACPU.sh HI6920ES_UDP";
            echo "       ./Make_LPS_STICK_ACPU.sh HI6920ES_UDPE5";
	    echo "       ./Make_LPS_STICK_ACPU.sh HI6920ES_E5776S";
            echo "       ./Make_LPS_STICK_ACPU.sh HI6920CS_P500";
            echo "       ./Make_LPS_STICK_ACPU.sh HI6920CS_SFT";
            echo "       ./Make_LPS_STICK_ACPU.sh P500_FPGA";
            echo "       ./Make_LPS_STICK_ACPU.sh HI6920CS_UDP";
            echo "       ./Make_LPS_STICK_ACPU.sh HI6920CS_STICK";
			echo "       ./Make_LPS_STICK_ACPU.sh HI6920CS_PAD";
            echo "       ./Make_LPS_STICK_ACPU.sh HI6920CS_E5776S";
esac


