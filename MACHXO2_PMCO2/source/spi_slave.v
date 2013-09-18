module spi_slave  (
	input clk_i,
	
	input spi_ssel_i,
	input spi_sck_i,
	input spi_mosi_i,
	output reg spi_miso_o = 1,
	
	input [7:0] di_i,  // parallel load data in (clocked in on rising edge of clk_i)
	input wren_i, // user data write enable
	output reg wr_ack_o = 0,                                       // write acknowledge
	output reg do_valid_o = 0,                                     // do_o data valid strobe, valid during one clk_i rising edge.
	output [7:0] do_o                  // parallel output (clocked out on falling clk_i) 
);

reg [7:0] shifter = 'h00;
reg [7:0] mosi_lat = 'h00;
reg [7:0] miso_lat = 'h00;
assign do_o = mosi_lat;


reg [2:0] bit_no = 0;
reg recieving = 0;

reg last_spi_sck = 0;
wire spi_sck_rising;
wire spi_sck_falling;
assign spi_sck_rising = !last_spi_sck & spi_sck_i;
assign spi_sck_falling = last_spi_sck & !spi_sck_i;

always @(posedge clk_i) begin
	if ( wren_i ) begin
		miso_lat <= di_i;
		wr_ack_o <= 1;
	end else begin
		wr_ack_o <= 0;
	end
	
	last_spi_sck <= spi_sck_i;
	
	if ( spi_ssel_i ) begin
		recieving <= 0;
	end else begin
		if ( !recieving ) begin // transmission started, latch in first byte and put bit on the line
			recieving <= 1;
			shifter[7:1] <= miso_lat[6:0]; // space for bit from host
			spi_miso_o <= miso_lat[7];
			bit_no <= 7;
		end
		if ( spi_sck_rising ) begin // latch in new data from host
			if (bit_no) begin
				bit_no <= bit_no - 1;
				shifter[0] <= spi_mosi_i;
			end else begin // last bit recieved
				mosi_lat <= {shifter[7:1], spi_mosi_i};
				do_valid_o <= 1;
			end
		end
		if ( spi_sck_falling ) begin //shift out data from us
			shifter[7:1] <= shifter[6:0];
			spi_miso_o <= shifter[7];
		end
	end
end

endmodule