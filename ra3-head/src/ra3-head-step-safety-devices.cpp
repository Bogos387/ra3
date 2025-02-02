#include    "ra3-head.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RA3HeadMotor::stepSafetyDevices(double t, double dt)
{
    // Не работать в неактивной кабине
    if (!is_active)
        return;

    blok->setVoltage(Ucc);
    blok->setAlsnCode(alsn_info.code_alsn);
    blok->setRBstate(tumbler[BUTTON_RB].getState());
    blok->setRBSstate(tumbler[BUTTON_RBS].getState());
    blok->setRailCoord(railway_coord + dir * getLength() / 2.0);
    blok->setVelocity(wheel_omega[0] * wheel_diameter / 2.0);
    blok->setKeyEPK(epk->getStateKey());
    blok->step(t, dt);

    epk->setFeedlinePressure(main_res->getPressure());
    epk->setBrakepipePressure(pTM);
    epk->setControl(keys);
    epk->powerOn(blok->getEPKstate() || static_cast<bool>(forward_inputs[SME_EPK_STATE]));
    epk->step(t, dt);
}
