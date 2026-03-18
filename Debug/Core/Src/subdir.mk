################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/DS18B20.c \
../Core/Src/DS3231(CLK).c \
../Core/Src/LCD.c \
../Core/Src/keypad.c \
../Core/Src/main.c \
../Core/Src/mcp23017.c \
../Core/Src/ph.c \
../Core/Src/physical_controls.c \
../Core/Src/state_logic.c \
../Core/Src/state_machine.c \
../Core/Src/stm32g0xx_hal_msp.c \
../Core/Src/stm32g0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g0xx.c \
../Core/Src/turbidity.c \
../Core/Src/waterlevel_us.c 

OBJS += \
./Core/Src/DS18B20.o \
./Core/Src/DS3231(CLK).o \
./Core/Src/LCD.o \
./Core/Src/keypad.o \
./Core/Src/main.o \
./Core/Src/mcp23017.o \
./Core/Src/ph.o \
./Core/Src/physical_controls.o \
./Core/Src/state_logic.o \
./Core/Src/state_machine.o \
./Core/Src/stm32g0xx_hal_msp.o \
./Core/Src/stm32g0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g0xx.o \
./Core/Src/turbidity.o \
./Core/Src/waterlevel_us.o 

C_DEPS += \
./Core/Src/DS18B20.d \
./Core/Src/DS3231(CLK).d \
./Core/Src/LCD.d \
./Core/Src/keypad.d \
./Core/Src/main.d \
./Core/Src/mcp23017.d \
./Core/Src/ph.d \
./Core/Src/physical_controls.d \
./Core/Src/state_logic.d \
./Core/Src/state_machine.d \
./Core/Src/stm32g0xx_hal_msp.d \
./Core/Src/stm32g0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g0xx.d \
./Core/Src/turbidity.d \
./Core/Src/waterlevel_us.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G0B1xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Core/Src/DS3231(CLK).o: ../Core/Src/DS3231(CLK).c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G0B1xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/Src/DS3231(CLK).d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/DS18B20.cyclo ./Core/Src/DS18B20.d ./Core/Src/DS18B20.o ./Core/Src/DS18B20.su ./Core/Src/DS3231(CLK).cyclo ./Core/Src/DS3231(CLK).d ./Core/Src/DS3231(CLK).o ./Core/Src/DS3231(CLK).su ./Core/Src/LCD.cyclo ./Core/Src/LCD.d ./Core/Src/LCD.o ./Core/Src/LCD.su ./Core/Src/keypad.cyclo ./Core/Src/keypad.d ./Core/Src/keypad.o ./Core/Src/keypad.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mcp23017.cyclo ./Core/Src/mcp23017.d ./Core/Src/mcp23017.o ./Core/Src/mcp23017.su ./Core/Src/ph.cyclo ./Core/Src/ph.d ./Core/Src/ph.o ./Core/Src/ph.su ./Core/Src/physical_controls.cyclo ./Core/Src/physical_controls.d ./Core/Src/physical_controls.o ./Core/Src/physical_controls.su ./Core/Src/state_logic.cyclo ./Core/Src/state_logic.d ./Core/Src/state_logic.o ./Core/Src/state_logic.su ./Core/Src/state_machine.cyclo ./Core/Src/state_machine.d ./Core/Src/state_machine.o ./Core/Src/state_machine.su ./Core/Src/stm32g0xx_hal_msp.cyclo ./Core/Src/stm32g0xx_hal_msp.d ./Core/Src/stm32g0xx_hal_msp.o ./Core/Src/stm32g0xx_hal_msp.su ./Core/Src/stm32g0xx_it.cyclo ./Core/Src/stm32g0xx_it.d ./Core/Src/stm32g0xx_it.o ./Core/Src/stm32g0xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g0xx.cyclo ./Core/Src/system_stm32g0xx.d ./Core/Src/system_stm32g0xx.o ./Core/Src/system_stm32g0xx.su ./Core/Src/turbidity.cyclo ./Core/Src/turbidity.d ./Core/Src/turbidity.o ./Core/Src/turbidity.su ./Core/Src/waterlevel_us.cyclo ./Core/Src/waterlevel_us.d ./Core/Src/waterlevel_us.o ./Core/Src/waterlevel_us.su

.PHONY: clean-Core-2f-Src

