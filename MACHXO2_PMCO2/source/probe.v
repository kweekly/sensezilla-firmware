module probe(
	LEDs_out
);

output [8:1] LEDs_out;
reg [8:1] LEDs;

assign LEDs_out = !LEDs;

endmodule