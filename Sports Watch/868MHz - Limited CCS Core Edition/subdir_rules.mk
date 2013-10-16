################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430_4.1.4/bin/cl430" -vmspx -g -O4 --opt_for_speed=0 --define=__CCE__ --define=TI_CC_RF_FREQ  868 --define=ISM_EU --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/ti/ccsv5/tools/compiler/msp430_4.1.4/include" --include_path="C:/ti/ccsv5/msp430/include" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/include" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/driver" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/logic" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/bluerobin" --include_path="D:/Texas Instruments/NewChronos/CCS/Sports Watch/simpliciti" --diag_warning=225 --call_assumptions=0 --auto_inline=0 --gen_opt_info=2 --printf_support=minimal --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


