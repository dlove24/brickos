/*! \file   include/c++/Sensor.H
    \brief  C++ Sensor Class Interface
    \author Pat Welch (legOS@mousebrains.com)  
		 
    Defines interface to Sensors plugged into the RCX
*/
// The contents of this file are subject to the Mozilla Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License. You may obtain a copy of the License
// at http://www.mozilla.org/MPL/
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
// the License for the specific language governing rights and
// limitations under the License.
//
// This software was developed as part of the legOS project.
//
// Contributor: Pat Welch (legOS@mousebrains.com)

#ifndef _Sensor_H_
#define _Sensor_H_

#include <config.h>

#if defined(CONF_DSENSOR)

#include <dsensor.h>
#include <conio.h>

/**
 * \class Sensor Sensor.H c++/Sensor.H
 * Basic sensor interface.
 * This is the base class for all sensors.
 *
 * At construction time you specify the RCX connector pad [\ref #Port] to 
 * which the sensor is attached and whether it should be powered (active/on) 
 * or not (passive/off).
 *
 * The %Sensor class provides methods to:
 * \li read the value of the sensor [\ref #get, \ref #sample]
 *
 * \li power the sensor as needed [\ref #on, \ref #off, \ref #active, 
 *  \ref #passive, \ref #mode]
 *
 * \note Generally touch sensors [identical to Sensor but may be spelled
 * TouchSensor] are not powered. 
 * Light sensors [\ref LightSensor] may be powered or not (the values are 
 * smaller when they are powered and the sensor is more tolerant of ambient 
 * light) and Rotation sensors  [\ref RotationSensor] must be powered when 
 * sensing rotation.
 * \todo how does TemperatureSensor figure in here...?
 */
class Sensor {
public:
  /**
   * List of sensor identifiers (IDs).
   *
   *  S1, S2, S3 are connector pads 1, 2 and 3 on the RCX.
   *
   *  Battery is the internal sensor allowing a program to read
   *  the current battery voltage in milliVolts
   *
   * \short Possible Sensor Port Values
   * \see Battery
   */
  enum Port {
    S1 = 0,	//!<  RCX sensor port 1
    S2,		//!<  RCX sensor port 2
    S3,		//!<  RCX sensor port 3
    Battery	//!<  RCX battery sensor
  };

  /**
   * Construct a new sensor instance.
   *
   * At time of construction identify where the sensor is connected and
   *  whether it should be active or passive (default is passive).
   *
   * \param port The port to be associated with this instance of a sensor
   * \param makeActive T/F - Where true means make active (default is false, passive)
   * \see Sensor#Port
   */
  Sensor(const Port port, bool makeActive = false)
    : sensor((port == S1) ? SENSOR_1 : 
             (port == S2) ? SENSOR_2 :
             (port == S3) ? SENSOR_3 :
	     BATTERY) 
  {
    if (makeActive) active();    // Don't use mode here, since this saves memory
    else            passive();
  }

  /**
   * Destroy this sensor instance
   *
   *  \note Just before the sensor instance is destroyed the power to the
   *  input pad is turned off (in case is was active) thereby 
   *  conserving battery power.
   */
  ~Sensor() {off();}

  /**
   * Get the current sensor reading
   *
   * \return unsigned int - The current raw sensor value
   * \see Sensor#sample
   */
  unsigned int get() const {return sensor;}

  /**
   * Set the sensor (mode) to active or passive
   *
   * \param makeActive - T/F - Where true means to go active
   * (voltage is supplied to the connector pad)
   * \see Sensor#off, Sensor#on, Sensor#active, Sensor#passive
   */
  void mode(bool makeActive) const {
    if (makeActive) active();
    else            passive();
  }

  /**
   * Set the sensor to passive (turn off voltage to connector)
   * \see Sensor#active, Sensor#on, Sensor#off, Sensor#mode
   */
  void passive() const {ds_passive(&sensor);}

  /**
   * Set the sensor to active (provide voltage to connector)
   * \see Sensor#passive, Sensor#on, Sensor#off, Sensor#mode
   */
  void active() const {ds_active(&sensor);}

  /**
   * Turn the sensor on (provide voltage to connector)
   * \see Sensor#off, Sensor#active, Sensor#passive, Sensor#mode
   */
  void on () const {active();}

  /**
   * Turn the sensor off (turn off voltage to connector)
   * \see Sensor#on, Sensor#active, Sensor#passive, Sensor#mode
   */
  void off() const {passive();}

  /**
   * Quickly turn sensor off then back on.
   * \see Sensor#on, Sensor#off, Sensor#active, Sensor#passive, Sensor#mode
   */
  void strobe() const {off(); on();}

  /**
   * Turn sensor off, wait for {ms} milliSeconds, then then turn it back on.
   * \param ms - Number of milliSeconds to wait between off and back on
   * \see Sensor#on, Sensor#off, Sensor#active, Sensor#passive, Sensor#mode
   */
  void strobe(const int ms) const {off(); delay(ms); on();}

  /**
   * Get the average of {size} samples, waiting {wait} mSec between each sample
   * \param size - Number of samples to average (default = 10 samples)
   * \param wait - time (in mS) to wait between samples (default = 2mS)
   * \return unsigned int - The average of the sampled values
   * \see Sensor#get
   */
  unsigned int sample(unsigned int size = 10, int wait = 2) const {
    if (size == 0) size = 1;
    unsigned int sum(get());
    for (unsigned int i = 1; i < size; ++i) {
      sum += get();
      delay(wait);
    }
    return sum / size;
  }

protected:
  /**
   * The address of our sensor value
   */
  volatile unsigned int& sensor;
};

#else // CONF_DSENSOR
#warning Enable CONF_DSENSOR to use Sensor.H
#endif // CONF_DSENSOR
#endif // _Sensor_H_

