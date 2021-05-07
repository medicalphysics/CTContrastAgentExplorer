#pragma once
#include <QDoubleSpinBox>
#include <QLayout>
#include <QWidget>

class CalculatorSettingsWidget : public QWidget {
    Q_OBJECT
public:
    CalculatorSettingsWidget(QWidget* parent = nullptr);
    void calculate();
signals:
    void bloodVolumeChanged(const double BV);
    void cardiacOutputChanged(const double CO);
    void renalClearenceChanged(const double clearence);

private:
    bool m_sexIsMale = true;
    double m_patientWeight = 75;
    double m_patientHeight = 180;
    int m_patientAge = 45;
    double m_patientCreatinine = 100;
};

class RawSettingsWidget : public QWidget {
    Q_OBJECT
public:
    RawSettingsWidget(QWidget* parent = nullptr);
    void setUseCalculator(bool c);
    void setBloodVolume(double);
    void setCardiacOutput(double);
    void setRenalClearence(double);
signals:
    void bloodVolumeChanged(const double BV);
    void cardiacOutputChanged(const double CO);
    void renalClearenceChanged(const double clearence);

private:
    QDoubleSpinBox* bv_spin = nullptr;
    QDoubleSpinBox* co_spin = nullptr;
    QDoubleSpinBox* rc_spin = nullptr;
    bool m_useCalculator = true;
};

class ContrastSettingsWidget : public QWidget {
    Q_OBJECT
public:
    ContrastSettingsWidget(QWidget* parent = nullptr);
signals:
    void volumeChanged(const double);
    void timeChanged(const double);
    void concentrationChanged(const double);
};

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    SettingsWidget(QWidget* parent = nullptr);
signals:
    void bloodVolumeChanged(const double bloodVolume);
    void cardiacOutputChanged(const double cardiacOutput);
    void renalClearenceChanged(const double renalClearence);
    void contrastInjectionVolumeChanged(const double ir);
    void contrastInjectionTimeChanged(const double time);
    void contrastInjectionConcentrationChanged(const double c);
};
