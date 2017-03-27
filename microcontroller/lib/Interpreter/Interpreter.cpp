#include "Interpreter.h"


//
// public
//


Interpreter::Interpreter(Disk* disk, Chute* chute, Angle start_angle)
{
  _chute = chute;
  _disk = disk;
  _next_slot = NULL;
  _start_angle = start_angle;
  _is_setup = false;
}


void Interpreter::setup()
{
  _next_slot = _disk->first_slot();  
  _is_setup = true;
}


void Interpreter::tick()
{
	if (!_is_setup)
	  return;
  
  if (_next_slot->is_over(_start_angle))
  {
    _process_slot(_next_slot);
    _next_slot = _disk->next_slot(_next_slot);
  }
}


//
// private
//


void Interpreter::_process_slot(Slot* slot)
{
  Serial.println("slot: ");
  Serial.print("object ferromagnetic? ");
  Serial.println(slot->has_ferromagnetic_object);
  Serial.print("object conductive? ");
  Serial.println(slot->has_conductive_object);
  Serial.print("object heavy? ");
  Serial.println(slot->has_heavy_object);
  //Serial.print("object length of first solid gap: ");
  //Serial.println(slot->profile.length_of_first_solid_gap());
  Serial.print("average object profile sample: ");
  Serial.println(slot->profile.average_sample());
  
  Chute::ObjectType object_type;
  
  if (slot->profile.average_sample() > 250)
    object_type = Chute::ObjectType::Coin;
  else if (slot->has_ferromagnetic_object)
    object_type = Chute::ObjectType::SmallSteelNut;
  else if (slot->has_heavy_object)
    object_type = Chute::ObjectType::BigBrassNut;
  else if (slot->has_conductive_object)
    object_type = Chute::ObjectType::SmallBrassNut;
  //else if (slot->profile.length_of_first_solid_gap() > 10)
  //  object_type = Chute::ObjectType::Coin;
  else
    object_type = Chute::ObjectType::BigNylonNut;
  
  _chute->turn_to_cup_for(object_type);
}
