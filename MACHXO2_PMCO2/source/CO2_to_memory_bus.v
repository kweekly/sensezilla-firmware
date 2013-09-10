`include "addresses.h"
module CO2_to_memory_bus(
	clk_i,

	 membus_read_req_i,
	 membus_write_req_i,
	 membus_addr_i,
	 membus_data_o,
	 membus_data_i,
	 
	 // I2C signals
	 I2C_SCL,
	 I2C_SDA,
	 
	 //debugging
	 dbg_state,
	 dbg_bytecounter,
);
parameter membus_address = 8'h10;

input clk_i;
input membus_read_req_i, membus_write_req_i;
input [6:0] membus_addr_i;
input [7:0] membus_data_i;
output reg [7:0] membus_data_o;
output [3:0] dbg_state;
output [3:0] dbg_bytecounter;

inout I2C_SCL,I2C_SDA;

reg wb_cyc = 0;
reg wb_we = 0;

reg [7:0] wb_adr = 0;
wire wb_ack;

reg [7:0] wb_dat_wr = 0;
wire [7:0] wb_dat_rd;
wire i2c_irq;


I2CSPI EFBmod( 	.wb_clk_i(clk_i),  // clock for submodules
				.wb_rst_i(1'b0), // never wish to reset the wishbone logic
				.wb_cyc_i(wb_cyc),  // command enable
				.wb_stb_i(wb_cyc),  // only one wishbone device
				.wb_we_i(wb_we),  // write-enable
				.wb_adr_i(wb_adr), // address to read/write
				.wb_dat_i(wb_dat_wr), // data to write
				.wb_dat_o(wb_dat_rd), //data to read
				.wb_ack_o(wb_ack), // acknowledge signal from slave			
				.i2c2_irqo(i2c_rq),
				
				// i2c signals
				.i2c2_scl(I2C_SCL),
				.i2c2_sda(I2C_SDA)			
				
);

`define I2C_CMD0  'hd0
`define I2C_CMD1 'h22
`define I2C_CMD2  'h00
`define I2C_CMD3  'h08
`define I2C_CMD4  'h2a
`define nCmdBytes 5

`define I2C_WAIT_FOR_TRIGGER 	0
`define I2C_SEND_READ_CMD		1
`define I2C_WAIT_FOR_READ		2
`define I2C_READ_FROM_SENSOR	3

// delay chain
// first timer is for how often we should trigger
// lets sample every 500mS * 7MHz = 3,500,000
// need 24 bits

`define CO2_SAMPLE_RATE_TMR 3500000

// next timer is the approximate time it takes to read from the sensor = 20ms
`define CO2_DELAY_TIME 140000

// some helpful things
`define WISHBONE_WRITE(REG,DAT) wb_adr <= (REG); wb_dat_wr <= (DAT); wb_cyc <= 1; wait_for_wishbone <= 1; wb_we <= 1;
`define WISHBONE_READ(REG) wb_adr <= (REG); wb_cyc <= 1; wait_for_wishbone <= 1; wb_we <= 0; // data will be in wishbone_data
`define WAIT_FOR_TRRDY `WISHBONE_READ( `I2C_2_SR ); wait_for_trrdy <= 1;
`define WAIT_FOR_SRW `WISHBONE_READ( `I2C_2_SR ); wait_for_srw <= 1;

// state variables
reg [23:0] CO2_Timer_reg=`CO2_SAMPLE_RATE_TMR;
reg [3:0] I2C_byte_counter = 0;
reg [3:0] I2C_state = `I2C_WAIT_FOR_TRIGGER;
reg wait_for_wishbone = 0;
reg [7:0] wishbone_data;
reg wait_for_trrdy = 0;
reg next_wait_for_trrdy = 0;
reg wait_for_srw   = 0;
reg next_wait_for_srw = 0;
reg need_to_read_data = 0;

// the data retrieved
reg [7:0] CO2_status = 0;
reg [7:0] CO2_value_L = 0;
reg [7:0] CO2_value_H = 0;
reg [7:0] CO2_checksum = 0;

assign dbg_state[1:0] = I2C_state[1:0];
assign dbg_state[2] = wait_for_trrdy | wait_for_srw;
assign dbg_state[3] = wb_cyc;
assign dbg_bytecounter = I2C_byte_counter;

always @(posedge clk_i) begin
	if (wait_for_wishbone) begin
		if ( wb_ack ) begin // data is valid
			wishbone_data <= wb_dat_rd;
			wb_cyc <= 0;		
			wait_for_wishbone <= 0;
		end
	end else if (wait_for_trrdy) begin 	// assume register has been read
		if (wishbone_data[2]) begin
			wait_for_trrdy <= 0; // done
		end else begin // re-read the register
			wb_cyc <= 1;
			wait_for_wishbone <= 1;
		end
	end else if (wait_for_srw) begin 	// assume register has been read
		if (wishbone_data[4]) begin
			wait_for_srw <= 0; // done
		end else begin // re-read the register
			wb_cyc <= 1;
			wait_for_wishbone <= 1;
		end
	end else if ( next_wait_for_trrdy ) begin
		`WAIT_FOR_TRRDY;
		next_wait_for_trrdy <= 0;
	end else if ( next_wait_for_srw ) begin
		`WAIT_FOR_SRW;
		next_wait_for_srw <= 0;
	end else begin
		if ( I2C_state == `I2C_WAIT_FOR_TRIGGER ) begin
			if ( CO2_Timer_reg == 0 ) begin
				I2C_state <= `I2C_SEND_READ_CMD;
				I2C_byte_counter <= 6;
				`WAIT_FOR_TRRDY;
			end else begin
				CO2_Timer_reg <= CO2_Timer_reg - 1;
			end
		end else if ( I2C_state == `I2C_SEND_READ_CMD ) begin // assume transmitter is ready
			I2C_byte_counter <= I2C_byte_counter - 1;
			if ( I2C_byte_counter == 6 ) begin // send start bit
				`WISHBONE_WRITE( `I2C_2_CMDR, 8'h90 );
				next_wait_for_trrdy <= 1;
			end else if ( I2C_byte_counter == 0 ) begin // go to next state
				I2C_state <= `I2C_WAIT_FOR_READ;
				CO2_Timer_reg <= `CO2_DELAY_TIME;
				`WISHBONE_WRITE( `I2C_2_CMDR, 8'h40 ); // send stop bit
			end else begin // 1 through 5 (5 bytes)
				case(I2C_byte_counter)
					5: begin `WISHBONE_WRITE( `I2C_2_TXDR, `I2C_CMD0 );	end
					4: begin `WISHBONE_WRITE( `I2C_2_TXDR, `I2C_CMD1 );	end
					3: begin `WISHBONE_WRITE( `I2C_2_TXDR, `I2C_CMD2 );	end
					2: begin `WISHBONE_WRITE( `I2C_2_TXDR, `I2C_CMD3 );	end
					default: begin `WISHBONE_WRITE( `I2C_2_TXDR, `I2C_CMD4 );end
				endcase
				next_wait_for_trrdy <= 1;
			end
		end else if ( I2C_state == `I2C_WAIT_FOR_READ ) begin
			if ( CO2_Timer_reg == 0) begin
				I2C_state <= `I2C_READ_FROM_SENSOR; // send start bit
				`WISHBONE_WRITE( `I2C_2_CMDR, 8'h90 );
				next_wait_for_srw <= 1;
				I2C_byte_counter <= 4;
			end else begin
				CO2_Timer_reg <= CO2_Timer_reg - 1;
			end
		end else if ( I2C_state == `I2C_READ_FROM_SENSOR ) begin
			if ( need_to_read_data == 1 ) begin
				`WISHBONE_READ( `I2C_2_RXDR );
				need_to_read_data <= 0;
			end else begin
				I2C_byte_counter <= I2C_byte_counter - 1;
				if ( I2C_byte_counter == 4 ) begin
					`WISHBONE_WRITE( `I2C_2_CMDR , 8'h20 ); // read byte
					need_to_read_data <= 1;
					next_wait_for_trrdy <= 1;
				end else if( I2C_byte_counter == 3) begin
					CO2_status <= wishbone_data;
					`WAIT_FOR_TRRDY;
					need_to_read_data <= 1;
				end else if(I2C_byte_counter == 2) begin
					CO2_value_H <= wishbone_data;
					`WAIT_FOR_TRRDY;
					need_to_read_data <= 1;
				end else if ( I2C_byte_counter == 1 ) begin
					CO2_value_L <= wishbone_data;
					`WISHBONE_WRITE( `I2C_2_CMDR, 8'h68 );
					next_wait_for_trrdy <= 1;
					need_to_read_data <= 1;
				end else begin // == 0
					CO2_checksum <= wishbone_data;
					I2C_state <= `I2C_WAIT_FOR_TRIGGER;
					CO2_Timer_reg <= `CO2_SAMPLE_RATE_TMR;
				end
			end
		end
	end
end


always @(posedge clk_i) begin
	if (membus_read_req_i) begin
		if (membus_addr_i == membus_address) begin
			membus_data_o <= CO2_value_L;
		end else if (membus_addr_i == membus_address + 1) begin
			membus_data_o <= CO2_value_H;
		end else begin
			membus_data_o <= 8'hZZ;
		end
	end else begin
		membus_data_o <= 8'hZZ;
	end
end

endmodule