# Flash memory

## AN 4835 - Production Flash Programming and Best Practices for Kinetis K- and L-series MCUs
- Interfaces con el controlador de flash:
	- JTAG o SWD, en este contexto es lo mismo así que hablan de SWD pero podría ser JTAG
	- EzPort -> específico para flash
- EzPort
	- The only function of EzPort is to interface to the flash
	- The processor is automatically halted in EzPort mode
	- Imprements commands that manage de FCCOB registers automatically
		- Del manual, pag 672: FCCOB (Flash Common Command Object) — A group of flash registers that are used to pass command, address, data, and any associated parameters to the memory controller in the FTFE module
- EzPort connection steps - muy importante!!!
- Erasing flash
	- Before a flash sector can be programmed it must be erased
	- Erase recommendations
		- most production flash programmers should only need to implement the **mass erase** and sector erase
		- The EzPort only has direct support for **mass erase**
	- Mass erase (está explicado antes, pero primero quería establecer cuál de los métodos nos era más relevante)
- Programming flash
	- nuestra Kinetis soporta section, longword (32-bit) y phrase (64-bit)
	- [Program section] is the recommended program command to use for production programming when available
	- The EzPort will automatically use the program section command for programming
	- Programming recommendations
		- When executing any flash command, always poll the appropiate status register waiting for completion of the command (flash FSTAT[CCIF] bit or EzPort status register's WIP bit) to determine if the command has completed before moving to the next command.

- Other programming considerations
	- Flash configuration field
		- 16-byte section at `0x400-0x40F`
		- Copied to flash registers at reset, so they set the default values for several flash settings
		- Freescale recommends that by default flash programmers write the following value to the flash configuration field: `0xFFFF FFFF | 0xFFFF_FFFF | 0xFFFF_FFFF | 0xFFFF_FFFE`. This value places the device in an unsecured state with no flash regions protected.
		- Changes to the flash configuration field will take effect at the next reset

- Common problems
	- not protecting the flash configuration field
		- take into consideration endianness
		- If the flash configuration field is programmed with a value that enables security and disables mass erase and backdoor key accesses, **then flash cannot be programmed.**
	- Interruption of an erase or program command **can lead to corruption of the flash contents**
	- Not using the Debug Request bit to prevent code from executing while programming the flash. If the core attempts to run code when establishing a debug connection or modifying flash, then the code can interfere. If there is no code programmed , then the processor will periodically reset due to core lockups. If there is code programmedin the device that modifies the power mode, clocking, or configuration of the SWD/JTAG pins, then that can prevent connection and/or cause corruption of the flash when a flash command is in process.
	- Applying voltages to I/O pins when the processor is not powered. If the pin VDIO or VIO specifications are violated, the processor can attempt a partial power up and/or puts the flash into an undefined state. **This can lead to corruption of flash contents**


### Paso a paso:
1. Conectar a EzPort (interfaz SPI)
2. Mass erase (verificar en status que puedo hacer mass erase, enviar comando, leer status hasta que WIP=0 (write in progress))
3. Programming flash



## K64 Sub-Family Reference Manual
### Chapter 28: Flash Memory Controller (FMC)
#### Initialization and application information
- **The FMC does not require user initialization.**  Flash acceleration features are enabled by default.
- The FMC has no visibility into flash memory erase and program cycles because **the Flash
Memory module manages them directly**


### Chapter 30: EzPort

#### External signal descriptions
- EPZ_CK: EzPort Clock
- EZP_CS: EzPort Chip Select (active low)
- EZP_D: EzPort Serial Data In
- EZP_Q: EzPort Serial Data Out

#### Modes of operation
- Enabled: EzPort steals access to the flash memory, preventing access from other cores or peripherals. The rest of the microcontroller is disabled to avoid conflicts
- Disabled: the rest of the microcontroller can access flash memory as normal


#### External signal descriptions
-Clock
	- EZP_D y EZP_CS are registered on the rising edge of EZP_CK
	- EZP_Q is driven on the falling edge of EZP_CLK
	- Read Data: f_{EZ_CLK} <= 1/8 * f_{SYS_CLK} (f_{SYS_CLK}=120MH --> f_{EZ_CLK} < 15MHz)
	- All other commands: f_{EZ_CLK} <= 1/2 * f_{SYS_CLK} --> f_{EZ_CLK} < 60MHz
- Chip select
	- While EZP_CS is asserted, if the microcontroller's reset out signal is negated, then EzPort is enabled out of reset. Otherwise EzPort is disabled.
	- After EzPort is enabled, asserting EZP_CS starts a serial data transfer, which continues until EZP_CS is negated again (resets the EzPort state machine)
- Serial data in
	- All commands, addresses and data are shifted in most significant bit first
	- When the EzPort is driving output data on EZP_Q, the data shifted in EZP_D is ignored
- Serial data out
	- All data is shifted out most significant bit first

#### Command definition
A partir de p.744 están las definiciones de los comandos, tiene hasta dibujitos de las señales. Los que nos interesan en particular:
- WREN (Write Enable): 0x06 
- WRDO (Write Disable): 0x04
- RDSR (Read Status Register): 0x05, 3 address bytes, 1 data byte
- SP (Flash Section Program): 0x02, 3 address bytes, 16-SECTION data byes (múltiplo de 16)
