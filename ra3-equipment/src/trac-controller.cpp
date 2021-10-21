#include    "trac-controller.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TracController::TracController(QObject *parent) : Device(parent)
  , mode_pos(0)
  , old_traction_key(false)
  , old_brake_key(false)
  , trac_min(0.17)
  , brake_min(0.26)
  , trac_level(0)
  , brake_level(0)
  , handle_pos(0.0)
  , omega_handle(0.5)
  , dir(0)
  , brakeTimer(new Timer)
  , tracTimer(new Timer)
{
    connect(brakeTimer, &Timer::process, this, &TracController::slotBrakeLevelProcess);
    connect(tracTimer, &Timer::process, this, &TracController::slotTracLevelProcess);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TracController::~TracController()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float TracController::getHandlePosition() const
{
    double level = mode_pos * 100 + trac_level - brake_level;

    return static_cast<float>(level) / 500.0f;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracController::preStep(state_vector_t &Y, double t)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracController::ode_system(const state_vector_t &Y,
                                    state_vector_t &dYdt,
                                    double t)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracController::load_config(CfgReader &cfg)
{
    QString secName = "Device";

    cfg.getDouble(secName, "trac_min", trac_min);
    cfg.getDouble(secName, "brake_min", brake_min);
    cfg.getDouble(secName, "omega_handle", omega_handle);

    brakeTimer->setTimeout(0.02);
    tracTimer->setTimeout(0.02);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracController::stepKeysControl(double t, double dt)
{
    processDiscretePositions(getKeyState(KEY_A), old_traction_key, 1);
    processDiscretePositions(getKeyState(KEY_D), old_brake_key, -1);

    if (mode_pos == -1)
    {
        traction.reset();
        dir = 0;

        if (!brakeTimer->isStarted())
            brakeTimer->start();

        if (getKeyState(KEY_A))
        {
            if (brake_level == 0)
            {
                mode_pos = 0;
                brakeTimer->stop();
                brake.reset();
            }
            else
            {
                dir = 1;
            }
        }

        if (getKeyState(KEY_D))
        {
            if (brake.getState())
                dir = -1;
        }
        else
        {
            brake.set();
        }
    }

    brakeTimer->step(t, dt);

    if (mode_pos == 1)
    {
        brake.reset();
        dir = 0;

        if (!tracTimer->isStarted())
            tracTimer->start();

        if (getKeyState(KEY_D))
        {
            if (trac_level == 0)
            {
                mode_pos = 0;
                tracTimer->stop();
                traction.reset();
            }
            else
            {
                dir = -1;
            }
        }

        if (getKeyState(KEY_A))
        {
            if (traction.getState())
                dir = 1;
        }
        else
        {
            traction.set();
        }
    }

    tracTimer->step(t, dt);

    old_traction_key = getKeyState(KEY_A);
    old_brake_key = getKeyState(KEY_D);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracController::processDiscretePositions(bool key_state, bool old_key_state, int dir)
{
    if (mode_pos != 0)
        return;

    if ( (key_state) && (!old_key_state) )
    {
        mode_pos += dir;
        mode_pos = cut(mode_pos, -1, 1);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracController::slotTracLevelProcess()
{
    trac_level += dir * mode_pos;

    trac_level = cut(trac_level, 0, 100);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracController::slotBrakeLevelProcess()
{
    brake_level += dir * mode_pos;

    brake_level = cut(brake_level, 0, 100);
}
