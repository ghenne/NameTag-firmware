If you have your arduino package installed at /opt/arduino-1.6.7 you can build as follows:

```
git clone <this repo url> NameTag-firmware
cd NameTag-firmware
mkdir build_dir
cd build_dir
export PATH=$PATH:/opt/arduino-1.6.7/hardware/tools/avr/bin
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake-avr/generic-gcc-avr.cmake -DAVR_MCU=atmega328 -DMCU_SPEED=8000000UL -DAVR_UPLOADTOOL_OPTIONS='-C/opt/arduino-1.6.7/hardware/tools/avr/etc/avrdude.conf' ..
make upload_main upload_eeprom
```

If you use another avr-gcc toolchain the step with setting the path may not be neccessary.

If you use another avrdude the -DAVR_UPLOADTOOL_OPTIONS... option has to be removed/replaced.
