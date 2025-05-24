package npc

import chisel3._
import chisel3.util._
import common.Consts.WORD_LEN

class Ebreak extends BlackBox with HasBlackBoxResource {
  val io = IO(new Bundle {
    val en = Input(Bool())
    val in = Input(UInt(WORD_LEN.W))
  })
  addResource("/dpi.v")
}
