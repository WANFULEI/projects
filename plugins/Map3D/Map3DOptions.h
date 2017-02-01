#pragma once
#include "QDialog"
#include "ui_map3DOptions.h"
#include "osgEarth\Extension"

class Map3DOptions :
	public QDialog, public Ui::Map3DOptions
{
	Q_OBJECT
public:
	Map3DOptions(QWidget *parent = 0);
	~Map3DOptions(void);

private slots:
	void on_sliderVerticalScale_sliderReleased();
	void on_checkSky_stateChanged ( int state);

private:
	osgEarth::Extension *m_sky;
};

