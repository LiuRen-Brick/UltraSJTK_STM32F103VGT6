################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Devices/App.c \
../Devices/dev_ad5160.c \
../Devices/dev_ad9833.c \
../Devices/dev_adc.c \
../Devices/dev_flash.c \
../Devices/dev_pwm.c \
../Devices/dev_screen.c 

OBJS += \
./Devices/App.o \
./Devices/dev_ad5160.o \
./Devices/dev_ad9833.o \
./Devices/dev_adc.o \
./Devices/dev_flash.o \
./Devices/dev_pwm.o \
./Devices/dev_screen.o 

C_DEPS += \
./Devices/App.d \
./Devices/dev_ad5160.d \
./Devices/dev_ad9833.d \
./Devices/dev_adc.d \
./Devices/dev_flash.d \
./Devices/dev_pwm.d \
./Devices/dev_screen.d 


# Each subdirectory must supply rules for building sources it contributes
Devices/%.o Devices/%.su Devices/%.cyclo: ../Devices/%.c Devices/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Devices -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Devices

clean-Devices:
	-$(RM) ./Devices/App.cyclo ./Devices/App.d ./Devices/App.o ./Devices/App.su ./Devices/dev_ad5160.cyclo ./Devices/dev_ad5160.d ./Devices/dev_ad5160.o ./Devices/dev_ad5160.su ./Devices/dev_ad9833.cyclo ./Devices/dev_ad9833.d ./Devices/dev_ad9833.o ./Devices/dev_ad9833.su ./Devices/dev_adc.cyclo ./Devices/dev_adc.d ./Devices/dev_adc.o ./Devices/dev_adc.su ./Devices/dev_flash.cyclo ./Devices/dev_flash.d ./Devices/dev_flash.o ./Devices/dev_flash.su ./Devices/dev_pwm.cyclo ./Devices/dev_pwm.d ./Devices/dev_pwm.o ./Devices/dev_pwm.su ./Devices/dev_screen.cyclo ./Devices/dev_screen.d ./Devices/dev_screen.o ./Devices/dev_screen.su

.PHONY: clean-Devices

