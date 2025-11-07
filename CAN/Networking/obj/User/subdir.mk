################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v30x_it.c \
../User/main.c \
../User/system_ch32v30x.c 

C_DEPS += \
./User/ch32v30x_it.d \
./User/main.d \
./User/system_ch32v30x.d 

OBJS += \
./User/ch32v30x_it.o \
./User/main.o \
./User/system_ch32v30x.o 


EXPANDS += \
./User/ch32v30x_it.c.234r.expand \
./User/main.c.234r.expand \
./User/system_ch32v30x.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32f -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -Wunused -Wuninitialized -g -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/SRC/Debug" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/SRC/Core" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/User" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/SRC/Peripheral/inc" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/arm" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/BLDC" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/distance_sensor" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/fast_sin" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/LCD" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/STEP" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/bsp/can" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/bsp/gpio" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/bsp/uart" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/application" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/bsp" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/SW" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/table" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/application/RB" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/bsp/exti" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/bsp/tim" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/IR" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/WT61" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/Architecture/module/TOF" -I"c:/Users/ton/Desktop/CH32/EVT/EVT/EXAM/CAN/Networking/NEW" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

