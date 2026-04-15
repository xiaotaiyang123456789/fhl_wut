################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Application/Src/SPI_LCD.c \
../User/Application/Src/can_app.c \
../User/Application/Src/key_app.c \
../User/Application/Src/shell_app.c 

OBJS += \
./User/Application/Src/SPI_LCD.o \
./User/Application/Src/can_app.o \
./User/Application/Src/key_app.o \
./User/Application/Src/shell_app.o 

C_DEPS += \
./User/Application/Src/SPI_LCD.d \
./User/Application/Src/can_app.d \
./User/Application/Src/key_app.d \
./User/Application/Src/shell_app.d 


# Each subdirectory must supply rules for building sources it contributes
User/Application/Src/%.o User/Application/Src/%.su User/Application/Src/%.cyclo: ../User/Application/Src/%.c User/Application/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g1 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F105xC -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../User/Application/Inc -I../User/Severs/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-Application-2f-Src

clean-User-2f-Application-2f-Src:
	-$(RM) ./User/Application/Src/SPI_LCD.cyclo ./User/Application/Src/SPI_LCD.d ./User/Application/Src/SPI_LCD.o ./User/Application/Src/SPI_LCD.su ./User/Application/Src/can_app.cyclo ./User/Application/Src/can_app.d ./User/Application/Src/can_app.o ./User/Application/Src/can_app.su ./User/Application/Src/key_app.cyclo ./User/Application/Src/key_app.d ./User/Application/Src/key_app.o ./User/Application/Src/key_app.su ./User/Application/Src/shell_app.cyclo ./User/Application/Src/shell_app.d ./User/Application/Src/shell_app.o ./User/Application/Src/shell_app.su

.PHONY: clean-User-2f-Application-2f-Src

