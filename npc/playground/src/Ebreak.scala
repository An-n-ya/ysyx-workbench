package npc

import chisel3._
import chisel3.util._

/** Compute GCD using subtraction method. Subtracts the smaller from the larger until register y is zero. value in
  * register x is then the GCD
  */
class Ebreak extends BlackBox with HasBlackBoxInline {
  val io = IO(new Bundle {
    val en = Input(Bool())
  })
  setInline("Ebreak.v",
    """import "DPI-C" function void ebreak();
      |module Ebreak(
      |    input  en
      |);
      |always @* begin
      |  if (en) begin
      |    ebreak();
      |  end
      |end
      |endmodule
    """.stripMargin)
}

