# Flash

## Flash the firmware into the Crazyflie

### Press the power butter for 3 seconds to enter bootloader mode and then type `make cload` in the terminal.


### Automatically enter bootloader mode

* Make sure the Crazyflie is on
* In your terminal, run `CLOAD_CMDS="-w radio://0/80/2M" make cload`

It will connect to the Crazyflie with the specified address, put it in bootloader mode and flash the binary. This method is suitable for classroom situations.

Note: this method does not work if the Crazyflie does not start, for instance if the current flashed binary is corrupt. You will have to fall back to manually entering bootloader mode.