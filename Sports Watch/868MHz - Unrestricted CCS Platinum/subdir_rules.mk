################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.4/bin/cl430" --cmd_file="D:\Texas Instruments\NewChronos\CCS\Sports Watch\simpliciti\Applications\configuration\smpl_nwk_config.dat" --cmd_file="D:\Texas Instruments\NewChronos\CCS\Sports Watch\simpliciti\Applications\configuration\End Device\smpl_config.dat"  -vmspx -g -O4 --opt_for_speed=0 --define=__CCE__ --define=ISM_EU --define=__CC430F6137__ --define=MRFI_CC430 --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.4/include" --include_path="C:/ti/ccsv5/msp430/include" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/include" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/driver" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/logic" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/bluerobin" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Applications/application/End Device" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/bsp" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/bsp/boards" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/bsp/boards/CC430EM" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/bsp/boards/CC430EM/bsp_external" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/bsp/drivers" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/bsp/drivers/code" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/bsp/mcus" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/mrfi" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/mrfi/radios" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/mrfi/radios/family5" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/mrfi/smartrf" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/nwk" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti/Components/nwk_applications" --diag_warning=225 --call_assumptions=0 --auto_inline=0 --gen_opt_info=2 --printf_support=minimal --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


