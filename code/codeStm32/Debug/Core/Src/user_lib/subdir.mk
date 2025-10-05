################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/user_lib/app.c \
../Core/Src/user_lib/bh1750.c \
../Core/Src/user_lib/keypad.c \
../Core/Src/user_lib/lcd_i2c.c \
../Core/Src/user_lib/mk_dht11.c \
../Core/Src/user_lib/my_debug.c \
../Core/Src/user_lib/password_handle.c \
../Core/Src/user_lib/servo.c \
../Core/Src/user_lib/sim4g.c 

OBJS += \
./Core/Src/user_lib/app.o \
./Core/Src/user_lib/bh1750.o \
./Core/Src/user_lib/keypad.o \
./Core/Src/user_lib/lcd_i2c.o \
./Core/Src/user_lib/mk_dht11.o \
./Core/Src/user_lib/my_debug.o \
./Core/Src/user_lib/password_handle.o \
./Core/Src/user_lib/servo.o \
./Core/Src/user_lib/sim4g.o 

C_DEPS += \
./Core/Src/user_lib/app.d \
./Core/Src/user_lib/bh1750.d \
./Core/Src/user_lib/keypad.d \
./Core/Src/user_lib/lcd_i2c.d \
./Core/Src/user_lib/mk_dht11.d \
./Core/Src/user_lib/my_debug.d \
./Core/Src/user_lib/password_handle.d \
./Core/Src/user_lib/servo.d \
./Core/Src/user_lib/sim4g.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/user_lib/%.o Core/Src/user_lib/%.su Core/Src/user_lib/%.cyclo: ../Core/Src/user_lib/%.c Core/Src/user_lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-user_lib

clean-Core-2f-Src-2f-user_lib:
	-$(RM) ./Core/Src/user_lib/app.cyclo ./Core/Src/user_lib/app.d ./Core/Src/user_lib/app.o ./Core/Src/user_lib/app.su ./Core/Src/user_lib/bh1750.cyclo ./Core/Src/user_lib/bh1750.d ./Core/Src/user_lib/bh1750.o ./Core/Src/user_lib/bh1750.su ./Core/Src/user_lib/keypad.cyclo ./Core/Src/user_lib/keypad.d ./Core/Src/user_lib/keypad.o ./Core/Src/user_lib/keypad.su ./Core/Src/user_lib/lcd_i2c.cyclo ./Core/Src/user_lib/lcd_i2c.d ./Core/Src/user_lib/lcd_i2c.o ./Core/Src/user_lib/lcd_i2c.su ./Core/Src/user_lib/mk_dht11.cyclo ./Core/Src/user_lib/mk_dht11.d ./Core/Src/user_lib/mk_dht11.o ./Core/Src/user_lib/mk_dht11.su ./Core/Src/user_lib/my_debug.cyclo ./Core/Src/user_lib/my_debug.d ./Core/Src/user_lib/my_debug.o ./Core/Src/user_lib/my_debug.su ./Core/Src/user_lib/password_handle.cyclo ./Core/Src/user_lib/password_handle.d ./Core/Src/user_lib/password_handle.o ./Core/Src/user_lib/password_handle.su ./Core/Src/user_lib/servo.cyclo ./Core/Src/user_lib/servo.d ./Core/Src/user_lib/servo.o ./Core/Src/user_lib/servo.su ./Core/Src/user_lib/sim4g.cyclo ./Core/Src/user_lib/sim4g.d ./Core/Src/user_lib/sim4g.o ./Core/Src/user_lib/sim4g.su

.PHONY: clean-Core-2f-Src-2f-user_lib

