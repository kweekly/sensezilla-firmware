`include "addresses.h" 

 
module toplevel ( 
			chip_sleep, 
			
			PMinputs ,

			SPI_MOSI,
			SPI_MISO,
			SPI_CSN,
			SPI_SCLK,
			
			UART_TXD,
			UART_RXD,
			
			LEDs,
			
			dbg_pins
			);
input chip_sleep;
input [`numPMinputs-1:0] PMinputs;
output [8:1] LEDs;

input SPI_MOSI,SPI_CSN,SPI_SCLK;
output SPI_MISO;

output UART_TXD;
input UART_RXD;

wire [8:1] iLEDs;
assign LEDs = ~iLEDs;

output [5:0] dbg_pins;

wire sysclk;


// set up the system clock
defparam OSCH_inst.NOM_FREQ = "7.00"; 
OSCH OSCH_inst( .STDBY( chip_sleep ),
				.OSC(sysclk), .SEDSTDBY() );

// set up the main memory bus
wire membus_read_req;
wire membus_write_req;
wire [6:0] membus_addr;
wire [7:0] membus_data_out;
wire [7:0] membus_data_in;

// SPI controller
spi_to_memory_bus spimembus( 
	.clk_i(sysclk),
	
	// membus
	 .membus_read_req_o(membus_read_req),
	 .membus_write_req_o(membus_write_req),
	 .membus_addr_o(membus_addr),
	 .membus_data_o(membus_data_in),
	 .membus_data_i(membus_data_out),

	// external pins
	.spi_ssel_i(SPI_CSN),
	.spi_sck_i(SPI_SCLK),
	.spi_mosi_i(SPI_MOSI),
	.spi_miso_o(SPI_MISO)
	
);
// debug LEDs for membus
assign iLEDs[2] = membus_read_req;
assign iLEDs[3] = membus_write_req;
assign iLEDs[4] = !SPI_CSN;


 
// create the counters
generate 
genvar i; 
for ( i = 0; i < `numPMinputs; i = i + 1 ) begin : pmgen
	PM_counter #(`ADDR_PM_CHANNELS + i*4) asdf(
	  .clk_i(sysclk), 
	
		// membus
	 .membus_read_req_i(membus_read_req),
	 .membus_write_req_i(membus_write_req),
	 .membus_addr_i(membus_addr),
	 .membus_data_o(membus_data_out),
	 .membus_data_i(membus_data_in), 
	  
	 .pm_input(PMinputs[i]), 
	 .pm_latch(!SPI_CSN)
	 
	// .dbg_counting(iLEDs[8-i])
   ); 
end
endgenerate  

PM_counter #(`ADDR_PM_COUNTER) main_counter(
	  .clk_i(sysclk), 
	
		// membus
	 .membus_read_req_i(membus_read_req),
	 .membus_write_req_i(membus_write_req),
	 .membus_addr_i(membus_addr),
	 .membus_data_o(membus_data_out),
	 .membus_data_i(membus_data_in), 
	  
	 .pm_input(1'b0), 
	 .pm_latch(!SPI_CSN)
	 
	 //.dbg_counting(iLEDs[4])
);

// CO2 I2C interface
CO2_to_memory_bus CO2iface(
	  .clk_i(sysclk), 
	
		// membus
	 .membus_read_req_i(membus_read_req),
	 .membus_write_req_i(membus_write_req),
	 .membus_addr_i(membus_addr),
	 .membus_data_o(membus_data_out),
	 .membus_data_i(membus_data_in),
	 
	 .UART_TXD(UART_TXD),
	 .UART_RXD(UART_RXD),
	 
	 //.dbg_state(iLEDs[4:1]),
	 //.dbg_bytecounter(iLEDs[8:5]),
	 //.dbg_pins(dbg_pins)
	 .dbg_reading(iLEDs[1])
);			 			 

endmodule 