# Echosense-Firmware

The Echosense firmware runs on the OSHchip nrf51822 processor and handles all data processing on the device side as well as bluetooth communication.

## Setup
Setup is done the same as any mbed program

1. Install the [Mbed CLI](https://os.mbed.com/docs/mbed-os/v5.14/quick-start/offline-with-mbed-cli.html).
2. Clone this repository and CD into it.
3. Run `mbed sync` to download the dependencies.
4. Run `mbed target OSHCHIP-XTAL` to set the target board to OSHchip with a 32 kibihertz low frequency clock attached.
5. (Optional) Run `mbed export -i cmake_gcc_arm` to create cmake files for the project for development and compilation in an IDE.

## Compilation and Installation
Compilation is done using the `mbed compile` command. The resulting .hex file can be manually copied to the fake mass storage device produced by the programmer, or flashed using a command line program using the HID programmer interface. More information about programming mbed boards using DAPlink can be found [here](https://os.mbed.com/docs/mbed-os/v5.14/tools/daplink.html).
