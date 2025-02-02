#include    "ra3-head.h"

#include    <QDir>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RA3HeadMotor::initControlCircuit()
{
    bat110 = new Battery();
    bat110->read_custom_config(config_dir + QDir::separator() + "bat110");

    bat24 = new Battery();
    bat24->read_custom_config(config_dir + QDir::separator() + "bat24");

    KM_bat_110 = new Relay(3);
    KM_bat_110->read_custom_config(config_dir + QDir::separator() + "mk");
    KM_bat_110->setInitContactState(0, false);
    KM_bat_110->setInitContactState(1, false);
    KM_bat_110->setInitContactState(2, false);

    mpsu = new MPSU();
    mpsu->read_custom_config(config_dir + QDir::separator() + "mpsu");

    generator = new Generator();
    generator->read_custom_config(config_dir + QDir::separator() + "generator");

    aux_conv = new AuxiliaryConverter(config_dir);
    aux_conv->read_custom_config(config_dir + QDir::separator() + "aux-converter");
}
