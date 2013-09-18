module spi_to_memory_bus (
	clk_i,

	 membus_read_req_o,
	 membus_write_req_o,
	 membus_addr_o,
	 membus_data_o,
	 membus_data_i,
	 
	 spi_ssel_i,
	 spi_sck_i,
	 spi_mosi_i,
	 spi_miso_o	 
);
input clk_i;
 
output reg membus_read_req_o, membus_write_req_o;output reg [6:0] membus_addr_o = 1'b0;
output reg [7:0] membus_data_o;
input [7:0] membus_data_i;

reg [7:0] membus_data_o_local = 8'hZZ;
assign membus_data_i = membus_data_o_local;

input spi_ssel_i, spi_sck_i, spi_mosi_i;
output spi_miso_o;


 reg  [7:0] spi_data_load;
 wire  [7:0] spi_data_rcv;
 reg spi_write_en_pre = 0;
 reg spi_write_en = 0;
 
 wire spi_write_ack;  
 wire spi_data_valid;
 
//defparam SPI.N = 8; // 8-bit words
//defparam SPI.PREFETCH = 1; // when does the "lookahead signal" assert before data needs to be present (higher gives us more time to load it up)
spi_slave SPI (
	.clk_i(clk_i),
	
	.spi_ssel_i(spi_ssel_i),
	.spi_sck_i(spi_sck_i),
	.spi_mosi_i(spi_mosi_i),
	.spi_miso_o(spi_miso_o),
	
	.di_i(spi_data_load),  // parallel load data in (clocked in on rising edge of clk_i)
	.wren_i(spi_write_en), // user data write enable
	.wr_ack_o(spi_write_ack),                                       // write acknowledge
	.do_valid_o(spi_data_valid),                                     // do_o data valid strobe, valid during one clk_i rising edge.
	.do_o(spi_data_rcv)                    // parallel output (clocked out on falling clk_i) 
);


// Behavior 
reg addr_recieved = 0;
reg write_enable = 0;
/*
Protocol:

Register READ:
Master: CSLOW {Start Address}><  0xFF ><  0xFF >< 0xFF > CSHIGH
Slave :     XX         ><  {B1} ><  {B2} >< {B3} >

Register WRITE:
Master: CSLOW 0x80 | {Start Address}>< B1><B2><B3> ... CSHIGH
*/
always @(posedge clk_i) begin
	membus_write_req_o <= 0;
	membus_read_req_o <= 0;
	spi_write_en <= spi_write_en_pre;
	spi_write_en_pre <= 0;
	spi_data_load <= 8'hFF;
	
	if (spi_ssel_i) begin // no transmission
		membus_addr_o <= 0;
		addr_recieved <= 0;
	end else begin
		if (spi_data_valid) begin
			if (addr_recieved) begin
				if ( write_enable ) begin
					membus_data_o <= spi_data_rcv;
					membus_write_req_o <= 1;
				end else begin
					//spi_data_load <= membus_data_i;
					spi_data_load <= 'hA4;
					membus_read_req_o <= 1;
					spi_write_en_pre <= 1;
				end
				membus_addr_o <= membus_addr_o + 1;
			end else begin
				membus_addr_o <= spi_data_rcv[6:0];
				write_enable <= spi_data_rcv[7];
				if (!spi_data_rcv[7]) begin
					membus_read_req_o <= 1;
					spi_write_en_pre <= 1;
				end
				addr_recieved <= 1;
			end
		end
	end
end

reg [7:0] temp_reg = 'h00;

/// testing
always @(posedge clk_i) begin
	if (membus_read_req_o) begin
		case (membus_addr_o) 
			`ADDR_ID: membus_data_o_local <= `VER_ID;
			`ADDR_TEMP: membus_data_o_local <= temp_reg;
			default:
				membus_data_o_local <= 8'hZZ;
		endcase
	end
	if ( membus_write_req_o) begin
		case ( membus_addr_o)
			`ADDR_TEMP: temp_reg <= membus_data_o;
			default: ;
		endcase
	end	
end

endmodule