#include    "ra3-head.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RA3HeadMotor::stepOtherEquipment(double t, double dt)
{
    horn->setControl(keys);
    horn->step(t, dt);

    hydro_pump->setDiselOmega(disel->getOmega());
    hydro_pump->step(t, dt);

    blok->setAlsnCode(alsn_info.code_alsn);
    blok->setRBstate(tumbler[BUTTON_RB].getState());
    blok->setRBSstate(tumbler[BUTTON_RBS].getState());
    blok->setVelocity(wheel_omega[0] * wheel_diameter / 2.0);
    blok->setKeyEPK(epk->getStateKey());
    blok->step(t, dt);

    epk->setFeedlinePressure(main_res->getPressure());
    epk->setBrakepipePressure(pTM);
    epk->setControl(keys);
    epk->powerOn(blok->getEPKstate() || static_cast<bool>(forward_inputs[SME_EPK_STATE]));
    epk->step(t, dt);
}
