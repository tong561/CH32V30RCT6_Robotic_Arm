################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Architecture/bsp/can/can.c 

C_DEPS += \
./Architecture/bsp/can/can.d 

OBJS += \
./Architecture/bsp/can/can.o 


EXPANDS += \
./Architecture/bsp/can/can.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
Architecture/bsp/can/%.o: ../Architecture/bsp/can/%.c
	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32f -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -Wunused -Wuninitialized -g -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Debug" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Core" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/User" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Peripheral/inc" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/arm" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/BLDC" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/distance_sensor" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/fast_sin" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/LCD" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/STEP" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/bsp/can" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/bsp/gpio" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/bsp/uart" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/application" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/bsp" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/SW" -I"e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/CAN/Networking/Architecture/module/table" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

