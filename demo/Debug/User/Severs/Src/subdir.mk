################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Severs/Src/debug_console.c \
../User/Severs/Src/soft_spi.c 

OBJS += \
./User/Severs/Src/debug_console.o \
./User/Severs/Src/soft_spi.o 

C_DEPS += \
./User/Severs/Src/debug_console.d \
./User/Severs/Src/soft_spi.d 


# Each subdirectory must supply rules for building sources it contributes
User/Severs/Src/%.o User/Severs/Src/%.su User/Severs/Src/%.cyclo: ../User/Severs/Src/%.c User/Severs/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g1 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F105xC -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../User/Application/Inc -I../User/Severs/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-Severs-2f-Src

clean-User-2f-Severs-2f-Src:
	-$(RM) ./User/Severs/Src/debug_console.cyclo ./User/Severs/Src/debug_console.d ./User/Severs/Src/debug_console.o ./User/Severs/Src/debug_console.su ./User/Severs/Src/soft_spi.cyclo ./User/Severs/Src/soft_spi.d ./User/Severs/Src/soft_spi.o ./User/Severs/Src/soft_spi.su

.PHONY: clean-User-2f-Severs-2f-Src

