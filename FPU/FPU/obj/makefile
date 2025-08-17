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
FPU.hex \

SECONDARY_LIST += \
FPU.lst \

SECONDARY_SIZE += \
FPU.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: FPU.elf secondary-outputs

# Tool invocations
FPU.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imafcxw -mabi=ilp32f -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant -Wunused -Wuninitialized -g -T "e:/desktop/上海理疗机械臂金主/CH32V307EVT (1)/EVT/EXAM/SRC/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Xlinker --print-memory-usage -Wl,-Map,"FPU.map" --specs=nano.specs --specs=nosys.specs -o "FPU.elf" $(OBJS)  $(LIBS) -lprintfloat
FPU.hex: FPU.elf
	@	riscv-none-embed-objcopy -O ihex "FPU.elf" "FPU.hex"
FPU.lst: FPU.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "FPU.elf" > "FPU.lst"
FPU.siz: FPU.elf
	riscv-none-embed-size --format=berkeley "FPU.elf"

# Other Targets
clean:
	-$(RM) $(OBJS) $(EXPANDS) $(CALLGRAPH_DOT) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) FPU.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets