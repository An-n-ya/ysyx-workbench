package npc

import chisel3._
import chisel3.util._

class Ebreak extends BlackBox with HasBlackBoxResource {
  val io = IO(new Bundle {
    val en = Input(Bool())
  })
  addResource("/dpi.v")
}
