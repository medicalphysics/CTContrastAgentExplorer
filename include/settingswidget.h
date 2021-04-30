#pragma once
#include <QDoubleSpinBox>
#include <QLayout>
#include <QWidget>

class SettingsWidget : public QWidget {
    Q_OBJECT
public:
    SettingsWidget(QWidget* parent = nullptr);

signals:
    void bloodVolumeChanged(const double bloodVolume);
    void cardiacOutputChanged(const double cardiacOutput);

protected:
    void updateBloodVolume();

private:
    QDoubleSpinBox* addSpinBox(QWidget* layout, const QString& description);
    double m_patientWeight = 0;
    double m_patientHeight = 0;
    double m_patientBloodVolume = 0;
    double m_patientCardiacOutput = 0;
    bool m_useBloodVolumeCalculator = true;
    bool m_patientIsMale = true;
    QDoubleSpinBox* m_bloodVolumeSpinBox = nullptr;
    QDoubleSpinBox* m_cardiacOutputSpinBox = nullptr;
};
