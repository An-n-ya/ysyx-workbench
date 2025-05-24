import "DPI-C" function void ebreak(int code);
module Ebreak(
    input  en,
    input  [31:0] in
);
always @* begin
  if (en) begin
    ebreak(in);
  end
end
endmodule
