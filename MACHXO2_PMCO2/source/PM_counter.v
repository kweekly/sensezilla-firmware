module PM_counter (
	clk_i,

	 membus_read_req_i,
	 membus_write_req_i,
	 membus_addr_i,
	 membus_data_o,
	 membus_data_i,

	 pm_input, 
	 pm_latch,
	 
	 dbg_counting
);
parameter membus_address = 8'h10;

input clk_i;
input membus_read_req_i, membus_write_req_i;
input [6:0] membus_addr_i;
input [7:0] membus_data_i;
input pm_input;
input pm_latch;

output reg [7:0] membus_data_o = 8'hZZ;

output reg dbg_counting;

reg [31:0] counter_val = 32'b0;
reg [31:0] latched_val = 32'b0;

reg pm_latch_ED = 1'b0;


always @(posedge clk_i) begin
	if (membus_read_req_i) begin
		if (membus_addr_i == membus_address) begin
			membus_data_o <= latched_val[7:0];
		end else if (membus_addr_i == membus_address + 1) begin
			membus_data_o <= latched_val[15:8];
		end else if (membus_addr_i == membus_address + 2) begin
			membus_data_o <= latched_val[23:16];
		end else if (membus_addr_i == membus_address + 3) begin
			membus_data_o <= latched_val[31:24];
		end else begin
			membus_data_o <= 8'hZZ;
		end
	end else begin
		membus_data_o <= 8'hZZ;
	end
	
	if ( pm_latch && !pm_latch_ED ) begin
		latched_val <= counter_val;
		if (!pm_input) begin
			counter_val <= 32'b1;
		end else begin
			counter_val <= 32'b0;
		end
	end else if ( !pm_input ) begin
		counter_val <= counter_val + 1;
	end
	
	if (!pm_input) begin
		dbg_counting <= 1;
	end else begin
		dbg_counting <= 0;
	end
		
	
	pm_latch_ED <= pm_latch;
end

 
endmodule