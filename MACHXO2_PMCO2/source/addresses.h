`ifndef _addresses_h
`define _addresses_h


`define numPMinputs 4

// ID register
`define ADDR_ID					7'h01
`define VER_ID					8'h11

// Temp register for testing
`define ADDR_TEMP			    7'h02

// Particle counter channels
// numPMcounters * 4 = 16
`define ADDR_PM_CHANNELS		7'h10

// Main counting channel
`define ADDR_PM_COUNTER			7'h30

// CO2 Data 
`define numCO2inputs 3
// numCO2 * 2 = 6
`define ADDR_CO2_CHANNELS		7'h40

`define NUM_MEMBUS_PORTS		(`numPMinputs + `numCO2inputs + 2)

`endif