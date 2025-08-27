################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/arm.c \
../User/arm_instance.c \
../User/ch32v30x_it.c \
../User/main.c \
../User/system_ch32v30x.c \
../User/table.c 

C_DEPS += \
./User/arm.d \
./User/arm_instance.d \
./User/ch32v30x_it.d \
./User/main.d \
./User/system_ch32v30x.d \
./User/table.d 

OBJS += \
./User/arm.o \
./User/arm_instance.o \
./User/ch32v30x_it.o \
./User/main.o \
./User/system_ch32v30x.o \
./User/table.o 


EXPANDS += \
./User/arm.c.234r.expand \
./User/arm_instance.c.234r.expand \
./User/ch32v30x_it.c.234r.expand \
./User/main.c.234r.expand \
./User/system_ch32v30x.c.234r.expand \
./User/table.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32f -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -Wunused -Wuninitialized -g -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Debug" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Core" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/User" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Peripheral/inc" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/arm" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/BLDC" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/distance_sensor" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/fast_sin" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/LCD" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/STEP" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/bsp/can" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/bsp/gpio" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/bsp/uart" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

