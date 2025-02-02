#include    "disel.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Disel::Disel(QObject *parent) : Device(parent)
  , V_oil(1.5)
  , Q_emn(0.0)
  , M_sg(0.0)
  , M_fg(0.0)
  , M_gen(0.0)
  , Mc(0.0)
  , ip(3.3)
  , J_shaft(1.0)
  , is_fuel_ignition(false)
  , state_mv6(false)
  , old_state_mv6(false)
  , state_vtn(false)
  , n_ref(800.0)
  , n_ref_prev(n_ref)
  , Q_max(0.166)
  , Q_fuel(0.0)
  , M_d(0.0)
  , omega_min(19.9)
  , start_time(3.0)
  , timer(new Timer)
  , fuel_pressure(0.0)
  , delta_omega(0.0)
  , pos_count(0)
  , soundName("pos0")
  , fuel_level(0.0)
  , name("d1")
{
    std::fill(K.begin(), K.end(), 0.0);

    K[0] = 15.0;
    K[1] = 0.0774;
    K[2] = K[3] = 1.7e5;
    K[4] = 0.01;

    connect(timer, &Timer::process, this, &Disel::slotFuelIgnition);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Disel::~Disel()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::step(double t, double dt)
{
    timer->step(t, dt);
    Device::step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::setQ_emn(double Q_emn)
{
    this->Q_emn = Q_emn;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::setStarterTorque(double M_sg)
{
    this->M_sg = M_sg;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    if (!is_fuel_ignition)
    {
        if (static_cast<bool>(hs_p(Y[1] - omega_min)))
        {
            timer->start();
        }        
    }
    else
    {
        is_fuel_ignition = state_mv6 && (fuel_level >= 0.01);
    }

    delta_omega = n_ref * Physics::PI / 30.0 - Y[1];

    Y[2] = cut(Y[2], 0.0, 1.0);

    double s1 = (K[4] * delta_omega + K[5] * Y[2]) * static_cast<double>(state_mv6);

    double u = cut(s1, 0.0, 1.0);

    double Q1 = u * static_cast<double>(state_vtn) * Q_max;

    double Q2 = u * Q_max;

    Q_fuel = K[6] * (Q1 + Q2);

    double M1 = K[2] * Q1;

    double M2 = K[3] * Q2;

    M_d = (M1 + M2) * static_cast<double>(is_fuel_ignition);

    //switchDiselSound(n_ref);

    emit soundSetPitch(soundName, static_cast<float>(getShaftFreq() / 800));
    emit soundSetVolume(soundName, static_cast<int>(Y[1] * 100.0 / 83.8));

    if (old_state_mv6)
    {
        if (state_mv6 != old_state_mv6)
        {
            emit soundStop(soundName);
            emit soundPlay(name + "-stop");
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t)
{
    Q_UNUSED(t)

    // Давление масла в системе смазки
    dYdt[0] = (Q_emn + K[1] * Y[1] - K[0] * Y[0]) / V_oil;

    double Mr = cut(Mc * Y[1] / 20.0, -Mc, Mc);

    // Частота вращения коленчатого вала
    dYdt[1] = (M_d + ip * M_sg - ip * M_fg - M_gen - Mr) / J_shaft;

    dYdt[2] = delta_omega;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::load_config(CfgReader &cfg)
{
    QString secName = "Device";

    cfg.getDouble(secName, "V_oil", V_oil);
    cfg.getDouble(secName, "Mc", Mc);
    cfg.getDouble(secName, "J_shaft", J_shaft);
    cfg.getDouble(secName, "ip", ip);
    cfg.getDouble(secName, "Qmax", Q_max);
    cfg.getDouble(secName, "omega_min", omega_min);
    cfg.getDouble(secName, "start_time", start_time);
    cfg.getString(secName, "Name", name);

    soundName = name + "-pos0";

    timer->firstProcess(false);
    timer->setTimeout(start_time);

    for (size_t i = 0; i < K.size(); ++i)
    {
        QString param = QString("K%1").arg(i);
        cfg.getDouble(secName, param, K[i]);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::switchDiselSound(double n_ref)
{
    double dn = pf(n_ref - 800);

    int i = 0;

    for (; i < MAX_POS; ++i)
    {
        if ( (dn >= i * 425) && (dn < (i+1) * 425) )
        {
            pos_count = i;
            break;
        }
    }

    QString newSoundName = name + QString("-pos%1").arg(i);

    if (newSoundName != soundName)
    {
        emit soundStop(soundName);
        soundName = newSoundName;
        emit soundPlay(soundName);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Disel::slotFuelIgnition()
{
    is_fuel_ignition = state_mv6 && static_cast<bool>(hs_p(fuel_pressure - 0.1));
    soundName = name + "-pos0";
    emit soundPlay(soundName);
    timer->stop();
}
