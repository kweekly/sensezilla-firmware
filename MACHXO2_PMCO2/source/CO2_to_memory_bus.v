`include "addresses.h"
module CO2_to_memory_bus(
	clk_i,

	 membus_read_req_i,
	 membus_write_req_i,
	 membus_addr_i,
	 membus_data_o,
	 membus_data_i,
	 
	 // UART signals
	 UART_TXD,
	 UART_RXD,
	 
	 //debugging
	 dbg_state,
	 dbg_bytecounter,
	 dbg_pins,
	 dbg_reading
);
parameter membus_address = 8'h10;

input clk_i;
input membus_read_req_i, membus_write_req_i;
input [6:0] membus_addr_i;
input [7:0] membus_data_i;
output reg [7:0] membus_data_o;
output [3:0] dbg_state;
output [3:0] dbg_bytecounter;
output [5:0] dbg_pins;
output dbg_reading;

input UART_RXD;
output UART_TXD;


reg uart_tx_en = 0;
reg [7:0] uart_txdata = 0;
wire uart_rx_int;
wire [7:0] uart_rxdata_wire;
reg [7:0] uart_rxdata = 0; 
wire uart_is_transmitting;
wire uart_rx_error;
wire uart_rx_recieving;

/// UART submodule
uart uart_inst(
    .clk(clk_i), // The master clock for this module
    .rx(UART_RXD), // Incoming serial line
    .tx(UART_TXD), // Outgoing serial line
    .transmit(uart_tx_en), // Signal to transmit
    .tx_byte(uart_txdata), // Byte to transmit
    .received(uart_rx_int), // Indicated that a byte has been received.
    .rx_byte(uart_rxdata_wire), // Byte received
	.recv_error(uart_rx_error), //error recieving
	.is_receiving(uart_rx_recieving),
    
    .is_transmitting(uart_is_transmitting), // Low when transmit line is idle.
	.dbg_pins(dbg_pins)
);


// delay chain
// first timer is for how often we should trigger
// lets sample every 1s
// need 24 bits
`define CO2_SAMPLE_RATE_TMR 7000000

// time before we give up and try again 1s = 7M
`define CO2_TIMEOUT 		7000000

`define CO2_STATE_WAIT_FOR_TRIGGER		2'b01
`define CO2_STATE_SENDING_CMD			2'b10
`define CO2_STATE_RECIEVING_RESPONSE	2'b11

// state variables
reg [23:0] CO2_timer_reg=`CO2_SAMPLE_RATE_TMR;
reg [7:0] CO2_byte_counter = 0;
reg [1:0] CO2_state = `CO2_STATE_WAIT_FOR_TRIGGER;
assign dbg_state[1:0] = CO2_state;
assign dbg_bytecounter[2:0] = CO2_byte_counter[2:0];
reg wait_for_txrdy = 0;
reg wait_for_txrdy_hold = 0;
reg wait_for_rx = 0;
assign dbg_state[2] = wait_for_rx;
assign dbg_state[3] = wait_for_txrdy;
assign dbg_bytecounter[3] =  uart_rx_recieving;
assign dbg_reading = (CO2_state != `CO2_STATE_WAIT_FOR_TRIGGER);

// the data retrieved
reg [7:0] CO2_value_L = 0;
reg [7:0] CO2_value_H = 0;

reg [7:0] CO2_POLL_CMD[0:7];// = '{'hFE,'h44,'h00,'h08,'h02,'h9F,'h25};
`define CMDLEN 7
`define CO2_VAL_IDX 3

initial begin
	CO2_POLL_CMD[6] = 'hFE;
	CO2_POLL_CMD[5] = 'h44;
	CO2_POLL_CMD[4] = 'h00;
	CO2_POLL_CMD[3] = 'h08;
	CO2_POLL_CMD[2] = 'h02;
	CO2_POLL_CMD[1] = 'h9F;
	CO2_POLL_CMD[0] = 'h25;
end

always @(posedge clk_i) begin
	if (uart_tx_en) begin // only raised for one clock cycle
		uart_tx_en <= 0;
	end
	
	if ( wait_for_txrdy_hold ) begin
		wait_for_txrdy_hold <= 0;
		wait_for_txrdy <= 1;
	end else if (wait_for_txrdy) begin
		if ( !uart_is_transmitting ) begin
			wait_for_txrdy <= 0;
		end else begin
			wait_for_txrdy <= 1;
		end
	end else if (wait_for_rx) begin
		if ( CO2_timer_reg == 0) begin
			CO2_state <= `CO2_STATE_SENDING_CMD;
			CO2_byte_counter <= `CMDLEN - 1;
			wait_for_txrdy_hold <= 1;
			wait_for_rx <= 0;
		end else begin
			CO2_timer_reg <= CO2_timer_reg - 1;
		end
		
		if ( uart_rx_int ) begin
			uart_rxdata <= uart_rxdata_wire;
			wait_for_rx <= 0;
		end
	end else begin
		case(CO2_state)
			`CO2_STATE_WAIT_FOR_TRIGGER : begin
				if (CO2_timer_reg == 0) begin
					CO2_state <= `CO2_STATE_SENDING_CMD;
					CO2_byte_counter <= `CMDLEN - 1;
					wait_for_txrdy_hold <= 1;
				end else begin
					CO2_timer_reg <= CO2_timer_reg - 1;
				end 
				end
			`CO2_STATE_SENDING_CMD : begin
					CO2_byte_counter <= CO2_byte_counter - 1;
					uart_txdata <= CO2_POLL_CMD[CO2_byte_counter];
					uart_tx_en <= 1;
					wait_for_txrdy_hold <= 1;
					if ( CO2_byte_counter == 0 ) begin
						CO2_state <= `CO2_STATE_RECIEVING_RESPONSE;
						wait_for_rx <= 1;
						CO2_timer_reg <= `CO2_TIMEOUT;
						CO2_byte_counter <= `CMDLEN - 1;
					end
				end
			`CO2_STATE_RECIEVING_RESPONSE:	begin
					CO2_byte_counter <= CO2_byte_counter - 1;

					case(CO2_byte_counter)
						`CO2_VAL_IDX: begin 
							CO2_value_H <= uart_rxdata;
						end
						(`CO2_VAL_IDX - 1): begin
							CO2_value_L <= uart_rxdata;
							CO2_state <= `CO2_STATE_WAIT_FOR_TRIGGER;
							CO2_timer_reg <= `CO2_SAMPLE_RATE_TMR;
						end
						default: begin
							wait_for_rx <= 1;
						end
					endcase
				end
			default: CO2_state <= `CO2_STATE_WAIT_FOR_TRIGGER;		
		endcase
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