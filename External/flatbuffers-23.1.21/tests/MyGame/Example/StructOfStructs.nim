#[ MyGame.Example.StructOfStructs
  Automatically generated by the FlatBuffers compiler, do not modify.
  Or modify. I'm a message, not a cop.

  flatc version: 23.1.21

  Declared by  : 
  Rooting type : MyGame.Example.Monster ()
]#

import Ability as MyGame_Example_Ability
import Test as MyGame_Example_Test
import flatbuffers

type StructOfStructs* = object of FlatObj
func a*(self: StructOfStructs): MyGame_Example_Ability.Ability =
  return MyGame_Example_Ability.Ability(tab: Vtable(Bytes: self.tab.Bytes, Pos: self.tab.Pos + 0))
func b*(self: StructOfStructs): MyGame_Example_Test.Test =
  return MyGame_Example_Test.Test(tab: Vtable(Bytes: self.tab.Bytes, Pos: self.tab.Pos + 8))
func c*(self: StructOfStructs): MyGame_Example_Ability.Ability =
  return MyGame_Example_Ability.Ability(tab: Vtable(Bytes: self.tab.Bytes, Pos: self.tab.Pos + 12))
proc StructOfStructsCreate*(self: var Builder, a_id: uint32, a_distance: uint32, b_a: int16, b_b: int8, c_id: uint32, c_distance: uint32): uoffset =
  self.Prep(4, 20)
  self.Prep(4, 8)
  self.Prepend(c_distance)
  self.Prepend(c_id)
  self.Prep(2, 4)
  self.Pad(1)
  self.Prepend(b_b)
  self.Prepend(b_a)
  self.Prep(4, 8)
  self.Prepend(a_distance)
  self.Prepend(a_id)
  return self.Offset()
