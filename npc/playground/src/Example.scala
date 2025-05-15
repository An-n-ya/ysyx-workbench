package example

import chisel3._

class Example extends Module {
  val io = IO(new Bundle {
    val a = Input(UInt(8.W))
    val b = Input(UInt(8.W))
    val f = Output(UInt(8.W))
  })

  io.f := io.a ^ io.b
}
