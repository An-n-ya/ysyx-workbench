import "DPI-C" function void ebreak();
module Ebreak(
    input  en
);
always @* begin
  if (en) begin
    ebreak();
  end
end
endmodule
