################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../box.cpp \
../fps_counter.cpp \
../inifile.cpp \
../main.cpp \
../signage.cpp \
../textures.cpp 

OBJS += \
./box.o \
./fps_counter.o \
./inifile.o \
./main.o \
./signage.o \
./textures.o 

CPP_DEPS += \
./box.d \
./fps_counter.d \
./inifile.d \
./main.d \
./signage.d \
./textures.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -O0 -g3 -Wunused-variable -c -fmessage-length=0 $$(pkg-config --cflags libxml++-2.6) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


