################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v30x_it.c \
../User/main.c \
../User/system_ch32v30x.c \
../User/table.c 

C_DEPS += \
./User/ch32v30x_it.d \
./User/main.d \
./User/system_ch32v30x.d \
./User/table.d 

OBJS += \
./User/ch32v30x_it.o \
./User/main.o \
./User/system_ch32v30x.o \
./User/table.o 


EXPANDS += \
./User/ch32v30x_it.c.234r.expand \
./User/main.c.234r.expand \
./User/system_ch32v30x.c.234r.expand \
./User/table.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32f -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -Wunused -Wuninitialized -g -I"e:/desktop/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Debug" -I"e:/desktop/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Core" -I"e:/desktop/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/FPU/FPU/User" -I"e:/desktop/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

