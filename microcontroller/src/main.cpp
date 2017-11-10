#include <Arduino.h>
#include <Wire.h>

#include "CoterminalAngle.h"
#include "AdafruitMotorShield.h"
#include "StepperMotor.h"

//#include "NutSorter/NutSorter.h"

#include "NutSorter/Profile.h"
#include "NutSorter/Slot.h"
#include "NutSorter/LightSensor.h"
#include "NutSorter/Carousel.h"
#include "NutSorter/Chute.h"
#include "NutSorter/Interpreter.h"
#include "NutSorter/FerromagneticSensor.h"
#include "NutSorter/FerromagneticMeasurement.h"
#include "NutSorter/ConductivitySensor.h"
#include "NutSorter/ConductivityMeasurement.h"
#include "NutSorter/ForceSensor.h"
#include "NutSorter/ForceMeasurement.h"
#include "NutSorter/ProfileMeasurement.h"
#include "NutSorter/Initializer.h"

// here we declare all the system's concurrent tasks

// all the angle offsets for every component can be adjusted here to
//   account for any changes in hardware
AdafruitMotorShield motor_shield;
AdafruitStepperMotor* stepper_motor_driver = motor_shield.getStepper(200, 2);

StepperMotor stepper_motor(stepper_motor_driver, 200, StepperMotor::StepType::Microstep, false);



NutSorter::Carousel carousel(&stepper_motor, CoterminalAngle(66.5)); // this offset may need to be adjusted in different lighting condtions 
NutSorter::Chute chute;

NutSorter::Initializer initializer(&carousel, CoterminalAngle(360.0));
NutSorter::FerromagneticMeasurement ferromagnetic_measurement(&carousel, CoterminalAngle(123.0), CoterminalAngle(125.5));
NutSorter::ConductivityMeasurement conductivity_measurement(&carousel, CoterminalAngle(179.0), CoterminalAngle(196.5));
NutSorter::ForceMeasurement force_measurement(&carousel, CoterminalAngle(230.0), CoterminalAngle(266.5));
NutSorter::ProfileMeasurement profile_measurement(&carousel, CoterminalAngle(295.0), CoterminalAngle(298.5));
NutSorter::Interpreter interpreter(&carousel, &chute, CoterminalAngle(349.0));


// call this in the event of an error
void end()
{
  while (true) {}
}


// setup every task (eg. pin modes)
// some return false to indicate an error
void setup()
{
  Serial.begin(9600);
  Serial.println("|reset");
  Serial.println("calibrating...");
  Serial.println("|calibrating");
  motor_shield.begin();
  carousel.setup();
  chute.setup();
  initializer.setup();
  if (!ferromagnetic_measurement.setup())
  {
    Serial.println("ferromagnetic measurer setup error...");
    Serial.println("|setup_error");
    end();
  }
  if (!conductivity_measurement.setup())
  {
    Serial.println("conductivity measurer setup error...");
    Serial.println("|setup_error");
    end();
  }
  force_measurement.setup();
  profile_measurement.setup();
  interpreter.setup();
  delay(5000);
  Serial.println("ready...");
  Serial.println("|running");
  carousel.start();
}


// this is the reactor loop
// no calls should block here!
// if they do, all other tasks will miss deadlines
// the tick() method is called for each task
//    often the task will simply return
//    sometimes the task will have work do to, it will quickly run and return
//    execution to the ractor loop for the next task
void loop()
{
  stepper_motor.tick();
  initializer.tick();
  ferromagnetic_measurement.tick();
  conductivity_measurement.tick();
  force_measurement.tick();
  profile_measurement.tick();
  interpreter.tick();
}
