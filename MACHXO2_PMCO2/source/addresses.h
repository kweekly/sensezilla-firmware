`ifndef _addresses_h
`define _addresses_h


`define numPMinputs 4
// Particle counter channels
// numPMcounters * 4 = 16
`define ADDR_PM_CHANNELS		7'h10

// Main counting channel
`define ADDR_PM_COUNTER			7'h20

// CO2 Data
`define ADDR_CO2_DATA_L			7'h21
`define ADDR_CO2_DATA_H			7'h22


// I2C wishbone registers
`define I2C_2_CR	'h4A
`define	I2C_2_CMDR	'h4B
`define I2C_2_TXDR	'h4E
`define I2C_2_SR	'h4F
`define I2C_2_RXDR	'h51
`endif