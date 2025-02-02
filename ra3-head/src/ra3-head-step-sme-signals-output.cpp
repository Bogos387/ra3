#include    "ra3-head.h"

#include <ra3-head.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RA3HeadMotor::stepSMESignalsOutput(double t, double dt)
{
    Q_UNUSED(t)
    Q_UNUSED(dt)

    // Сигнал включения контактора "Бортсеть" на ведомую секцию
    backward_outputs[SME_BWD_POWER_ON] = static_cast<float>(KM_bat_110->getContactState(2));

    // Сигнал включения топливного насоса на ведомом дизеле
    backward_outputs[SME_BWD_FUEL_PUMP] = static_cast<float>(mpsu->getOutputData().is_fuel_pump2_ON);

    // Сигнал включения стартера на ведомом дизеле
    backward_outputs[SME_BWD_STARTER_ON] = static_cast<float>(mpsu->getOutputData().is_starter2_ON);

    // Сигнал открытия топливного клапана на ведомом дизеле
    backward_outputs[SME_BWD_FUEL_VALVE_OPEN] = static_cast<float>(mpsu->getOutputData().is_fuel_valve2_open);

    // Сигнал запуска генератора на ведомой секции
    forward_outputs[SME_BWD_GENERATOR] = static_cast<float>(generator->isActive());

    // Передаем давление масла в ведущую секцию
    forward_outputs[SME_BWD_OIL_PRESS] = static_cast<float>(disel->getOilPressure());

    // Передаем обороты дизеля на ведущую секцию
    forward_outputs[SME_BWD_OMEGA] = static_cast<float>(disel->getShaftFreq());

    // Передаем сигнал включения топливного насоса на ведущую секцию
    forward_outputs[SME_BWD_FUEL_PUMP_ON] = static_cast<float>(fuel_pump->isStarted());

    // Передаем давление топлива в ведущую секцию
    forward_outputs[SME_BWD_FUEL_PRESS] = static_cast<float>(fuel_pump->getFuelPressure());

    // Сигнал запуска дизеля для включения ПСН в ведущей секции
    backward_outputs[SME_BWD_DISEL_STARTED] = static_cast<float>(mpsu->getOutputData().is_disel2_started);

    // Сигнал запуска компрессора на ведущую секцию
    forward_outputs[SME_BWD_COMPRESSOR] = static_cast<float>(motor_compr->isPowered());

    // Команда управления стояночным тормозом на ведущую секцию
    backward_outputs[SME_PARKING_BRAKE_ON] = static_cast<float>(tumbler[SWITCH_PARKING_BRAKE].getState());

    // Сигнал состояния стояночного тормоза на ведущую секцию
    forward_outputs[SME_PARKING_BRAKE_STATE] = static_cast<float>(brake_module->isParkingBraked());

    // Заданный уровень торможения ЭПТ на ведомую секцию
    backward_outputs[SME_REF_BRAKE_LEVEL] = km->getBrakeLevel();

    // Сигнал отпуска тормозов (КЭБ) на ведомую секцию
    backward_outputs[SME_BRAKE_RELEASE] = static_cast<float>(false);

    // Питание удерживающей катушки ЭПК
    backward_outputs[SME_EPK_STATE] = static_cast<float>(blok->getEPKstate());


    backward_outputs[SME_DISEL_FREQ] = static_cast<float>(mpsu->getOutputData().n_ref);

    backward_outputs[SME_REVERS_HANDLE] = km->getReversHandlePos();

    forward_outputs[SME_REVERS_STATE] = hydro_trans->getReversState();

    forward_outputs[SME_BWD_BC1] = brake_mech[FWD_TROLLEY]->getBrakeCylinderPressure();
    forward_outputs[SME_BWD_BC2] = brake_mech[BWD_TROLLEY]->getBrakeCylinderPressure();
    forward_outputs[SME_BWD_BRAKE_LEVEL] = hydro_trans->getBrakeLevel();

    // Состояние блок-контактов КМ
    backward_outputs[SME_IS_KM_ZERO] = static_cast<float>(km->isZero());
    backward_outputs[SME_IS_KM_TRACTION] = static_cast<float>(km->isTraction());
    backward_outputs[SME_IS_KM_BRAKE] = static_cast<float>(km->isBrake());

    // Уровень тяги и торможения с КМ для задней секции
    backward_outputs[SME_KM_BRAKE_LEVEL] = km->getBrakeLevel();
    backward_outputs[SME_KM_TRACTION_LEVEL] = km->getTractionLevel();

    backward_outputs[SME_IS_EMERGENCY_BRAKE] = static_cast<float>(
                emerg_brake_valve->isEmergencyBrake() ||
                km->isEmergencyBrake());

    // Сигнал наичия связи по как для ведущей секции
    forward_outputs[SME_BWD_CAN] = 1.0f;
}
