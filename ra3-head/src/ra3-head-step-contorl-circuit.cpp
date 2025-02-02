#include    "ra3-head.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RA3HeadMotor::stepControlCircuit(double t, double dt)
{
    U_bat_110 = bat110->getVoltage();
    Ucc = static_cast<double>(KM_bat_110->getContactState(1)) * U_bat_110;

    Icc = Ucc / 22.0;

    bat110->setLoadCurrent(Icc);
    bat110->setChargeVoltage(aux_conv->getU_110());
    bat110->step(t, dt);

    bat24->step(t, dt);

    // Включение контактора "Бортсеть"
    bool is_KM_bat_110 = tumbler[BUTTON_PWR_ON].getState() ||
            (tumbler[BUTTON_PWR_OFF].getState() && KM_bat_110->getContactState(0)) ||
           static_cast<bool>(forward_inputs[SME_BWD_POWER_ON]);

    KM_bat_110->setVoltage(U_bat_110 * static_cast<double>(is_KM_bat_110));
    KM_bat_110->step(t, dt);    

    // Главный генератор
    generator->setHydroStaticPress(hydro_pump->getPressure());
    generator->step(t, dt);

    // ПСН
    aux_conv->setPowerON(mpsu->getOutputData().is_disel1_started ||
                         static_cast<bool>(forward_inputs[SME_BWD_DISEL_STARTED]));

    aux_conv->setBatteryVoltage(Ucc);
    aux_conv->setInputVoltage(generator->getVoltage());
    aux_conv->step(t, dt);
}
