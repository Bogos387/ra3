#include    "blok.h"

#include    <QTextStream>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
BLOK::BLOK(QObject *parent) : Device(parent)
  , code_alsn(1)
  , old_code_alsn(1)
  , state_RB(false)
  , state_RB_old(false)
  , state_RBS(false)
  , state_RBS_old(false)
  , state_EPK(false)
  , v_kmh(0.0)
  , v(0.0)
  , delta_t(0.1)
  , v_count(0)
  , t_diff(0.0)
  , acceleration(0.0)
  , key_epk(false)
  , is_dislplay_ON(false)
  , check_vigilance(false)
  , safety_timer(new Timer(45.0, false))
  , beepTimer(new Timer(0.5, true))
  , beep_interval(0.5)
  , rail_coord(0.0)
  , train_length(0.0)
{
    epk_state.reset();

    connect(safety_timer, &Timer::process, this, &BLOK::onSafetyTimer);
    connect(beepTimer, &Timer::process, this, &BLOK::onBeepTimer);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
BLOK::~BLOK()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::step(double t, double dt)
{
    safety_timer->step(t, dt);
    beepTimer->step(t, dt);

    calc_acceleration(t, dt);

    Device::step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::loadSpeedsMap(QString path)
{
    QFile map_file(path);

    if (map_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!map_file.atEnd())
        {
            QByteArray line;
            line = map_file.readLine();

            QTextStream ss(&line);

            speed_limit_t limit;
            ss >> limit.coord >> limit.value;

            limits.push_back(limit);
        }

        if (dir < 0)
            std::reverse(limits.begin(), limits.end());

        for (size_t i = 0; i < limits.size() - 1; ++i)
        {
            if (limits[i+1].value > limits[i].value)
            {
                limits[i+1].coord += dir * train_length;
            }
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::preStep(state_vector_t &Y, double t)
{
    // Очищаем состояние ламп
    std::fill(lamps.begin(), lamps.end(), 0.0f);

    // Ничего не делаем при выключенном питании
    if (hs_n(U_pow - 100.0))
    {
        is_dislplay_ON = false;
        return;
    }

    is_dislplay_ON = true;

    speed_control();

    // Ничего не делаем при выключенном ЭПК
    if (!key_epk)
    {
        epk_state.reset();
        return;
    }

    if (code_alsn < old_code_alsn)
        epk_state.reset();

    alsn_process(code_alsn);

    if (code_alsn == 1)
    {
        if (v_kmh > 60.0)
        {
            epk_state.reset();
            return;
        }

        if ( (!safety_timer->isStarted()) && (v_kmh > 5) )
        {
            safety_timer->start();
        }
        else
        {
            safety_timer->stop();
        }
    }

    if (code_alsn == 2)
    {
        if (v_kmh > 60.0)
        {
            if (!safety_timer->isStarted())
                safety_timer->start();
        }
        else
        {
            safety_timer->stop();
        }
    }

    if (state_RB || state_RBS)
    {
        epk_state.set();
        safety_timer->stop();        
    }

    check_vigilance = !epk_state.getState();

    sounds_process();    
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::ode_system(const state_vector_t &Y,
                      state_vector_t &dYdt,
                      double t)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::load_config(CfgReader &cfg)
{
    QString secName = "Device";

    double safety_check_interval = 45.0;

    cfg.getDouble(secName, "SafetyCheckInterval", safety_check_interval);

    safety_timer->setTimeout(safety_check_interval);

    //cfg.getDouble(secName, "BeepInterval", beep_interval);

    //beepTimer->setTimeout(beep_interval);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::alsn_process(int code_alsn)
{
    switch (code_alsn)
    {
    case 0:
        {
            lamps[WHITE_LAMP] = 1.0f;

            break;
        }
    case 1:
        {
            lamps[RED_YELLOW_LAMP] = 1.0f;

            break;
        }

    case 2:
        {
            lamps[YELLOW_LAMP] = 1.0f;

            break;
        }

    case 3:
        {
            lamps[GREEN_LAMP1] = 1.0f;

            break;
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::sounds_process()
{
    if ( (state_RB && (!state_RB_old)) ||
         (state_RB_old && (!state_RB)) )
    {
        emit soundPlay("BLOK_RB");
    }

    if ( (state_RBS && (!state_RBS_old)) ||
         (state_RBS_old && (!state_RBS)) )
    {
        emit soundPlay("BLOK_RB");
    }

    state_RB_old = state_RB;
    state_RBS_old = state_RBS;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::calc_acceleration(double t, double dt)
{
    Q_UNUSED(t)

    if (v < 1e-4)
    {
        acceleration = 0;
        return;
    }

    if (t_diff >= delta_t)
    {
        v_i[v_count] = v;
        t_diff = 0;
        v_count++;
    }

    if (v_count >= v_i.size())
    {
        v_count = 0;
        t_diff = 0;

        acceleration = (3 * v_i[2] - 4 * v_i[1] + v_i[0]) / 2.0 / delta_t;
    }

    t_diff += dt;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::speed_control()
{
    calc_speed_limits();

    int V_kmh = qRound(v_kmh);

    if (V_kmh < current_limit - 3)
    {
        beepTimer->stop();
    }
    else
    {
        if (!beepTimer->isStarted())
            beepTimer->start();
    }

    if (V_kmh >= current_limit + 1)
    {
        epk_state.reset();
    }    
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::calc_speed_limits()
{
    if (!key_epk)
    {
        current_limit = next_limit = 300.0;
        return;
    }

    if (limits.empty())
    {
        current_limit = next_limit = v_max;
        return;
    }

    speed_limit_t cur_lim;
    speed_limit_t next_lim;

    findLimits(cur_lim, next_lim);

    double v_lim = 0;

    if (cur_lim.value > next_lim.value)
    {
        double a = 0.7;
        double dist = qAbs(next_lim.coord - cur_lim.coord);
        v_lim = sqrt( pow(cur_lim.value, 2) + 2 * a *dist);
    }
    else
    {
        v_lim = v_max;
    }

    current_limit = min(v_lim, cur_lim.value + 1);
    next_limit = next_lim.value + 1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::findLimits(speed_limit_t &cur_limit, speed_limit_t &next_limit)
{
    if (limits.empty())
        return;

    size_t left_idx = 0;
    size_t right_idx = limits.size() - 1;
    size_t idx = (left_idx + right_idx) / 2;

    while (idx != left_idx)
    {
        speed_limit_t limit = limits[idx];

        if (rail_coord <= limit.coord)
            right_idx = idx;
        else
            left_idx = idx;

        idx = (left_idx + right_idx) / 2;
    }

    cur_limit = limits[idx];

    if (idx < limits.size() - 1)
        next_limit = limits[idx + 1];
    else
        next_limit = speed_limit_t();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::onSafetyTimer()
{
    epk_state.reset();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BLOK::onBeepTimer()
{
    emit soundPlay("BLOK_RB");
}
