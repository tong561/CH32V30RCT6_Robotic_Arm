################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
EXTI0.hex \

SECONDARY_LIST += \
EXTI0.lst \

SECONDARY_SIZE += \
EXTI0.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: EXTI0.elf secondary-outputs

# Tool invocations
EXTI0.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "e:/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Xlinker --print-memory-usage -Wl,-Map,"EXTI0.map" --specs=nano.specs --specs=nosys.specs -o "EXTI0.elf" $(OBJS)  $(LIBS)
EXTI0.hex: EXTI0.elf
	@	riscv-none-embed-objcopy -O ihex "EXTI0.elf" "EXTI0.hex"
EXTI0.lst: EXTI0.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "EXTI0.elf" > "EXTI0.lst"
EXTI0.siz: EXTI0.elf
	riscv-none-embed-size --format=berkeley "EXTI0.elf"

# Other Targets
clean:
	-$(RM) $(OBJS) $(EXPANDS) $(CALLGRAPH_DOT) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) EXTI0.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets