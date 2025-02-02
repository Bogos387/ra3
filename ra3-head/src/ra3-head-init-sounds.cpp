#include    "ra3-head.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RA3HeadMotor::initSounds()
{
    connect(KM_bat_110, &Relay::soundPlay, this, &RA3HeadMotor::soundPlay);
    KM_bat_110->setSoundName("Relay");

    connect(horn, &TrainHorn::soundPlay, this, &RA3HeadMotor::soundPlay);
    connect(horn, &TrainHorn::soundStop, this, &RA3HeadMotor::soundStop);

    connect(fuel_pump, &ElectricFuelPump::soundPlay, this, &RA3HeadMotor::soundPlay);
    connect(fuel_pump, &ElectricFuelPump::soundStop, this, &RA3HeadMotor::soundStop);
    connect(fuel_pump, &ElectricFuelPump::soundSetPitch, this, &RA3HeadMotor::soundSetPitch);
    connect(fuel_pump, &ElectricFuelPump::soundSetVolume, this, &RA3HeadMotor::soundSetVolume);

    connect(starter, &Starter::soundPlay, this, &RA3HeadMotor::soundPlay);
    connect(starter, &Starter::soundStop, this, &RA3HeadMotor::soundStop);

    connect(disel, &Disel::soundPlay, this, &RA3HeadMotor::soundPlay);
    connect(disel, &Disel::soundStop, this, &RA3HeadMotor::soundStop);
    connect(disel, &Disel::soundSetPitch, this, &RA3HeadMotor::soundSetPitch);
    connect(disel, &Disel::soundSetVolume, this, &RA3HeadMotor::soundSetVolume);

    if (is_active)
    {
        fuel_pump->setSoundName("Fuel_Pump1");
        starter->setSoundName("Starter1");
    }
    else
    {
        fuel_pump->setSoundName("Fuel_Pump2");
        starter->setSoundName("Starter2");
    }

    connect(epk, &AutoTrainStop::soundPlay,
            this, &RA3HeadMotor::soundPlay);

    connect(epk, &AutoTrainStop::soundStop,
            this, &RA3HeadMotor::soundStop);

    tumbler[SWITCH_PARKING_BRAKE].setOnSoundName("Switcher");
    tumbler[SWITCH_PARKING_BRAKE].setOffSoundName("Switcher");
    connect(&tumbler[SWITCH_PARKING_BRAKE], &Trigger::soundPlay,
            this, &RA3HeadMotor::soundPlay);

    km->setMainHandleSoundName("KM_main");
    km->setReversHandleSoundName("KM_revers");
    connect(km, &TracController::soundPlay, this, &RA3HeadMotor::soundPlay);

    connect(brake_module, &BTO092::soundPlay, this, &RA3HeadMotor::soundPlay);

    connect(blok, &BLOK::soundPlay, this, &RA3HeadMotor::soundPlay);

    connect(emerg_brake_valve, &EmergencyBrakeValve::soundPlay,
            this, &RA3HeadMotor::soundPlay);

    connect(emerg_brake_valve, &EmergencyBrakeValve::soundStop,
            this, &RA3HeadMotor::soundStop);

    connect(emerg_brake_valve, &EmergencyBrakeValve::soundSetVolume,
            this, &RA3HeadMotor::soundSetVolume);

    connect(km, &TracController::soundPlay,
            this, &RA3HeadMotor::soundPlay);

    connect(km, &TracController::soundStop,
            this, &RA3HeadMotor::soundStop);

    connect(km, &TracController::soundSetVolume,
            this, &RA3HeadMotor::soundSetVolume);

    connect(kru, &KRU091::soundSetVolume,
            this, &RA3HeadMotor::soundSetVolume);
}
