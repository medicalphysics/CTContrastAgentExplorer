#include "settingswidget.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

#include <cmath>

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    //blood volume
    auto bv_group = new QGroupBox("Patient data", this);

    mainLayout->addWidget(bv_group);
    mainLayout->addStretch();
    auto bv_layout = new QVBoxLayout(bv_group);
    bv_group->setLayout(bv_layout);

    auto bv_calculatorBox = new QCheckBox(bv_group);
    bv_calculatorBox->setCheckState(Qt::Checked);
    auto bv_calculatorLabel = new QLabel(tr("Use blod volume calculator"));
    auto bv_calc_layout = new QHBoxLayout();
    bv_calc_layout->addWidget(bv_calculatorBox);
    bv_calc_layout->addWidget(bv_calculatorLabel);
    bv_calc_layout->addStretch();
    bv_layout->addLayout(bv_calc_layout);

    auto sex_group = new QGroupBox(tr("Patient sex"));
    auto sex_layout = new QHBoxLayout;
    auto sex_maleButton = new QRadioButton(tr("&Male"));
    auto sex_femaleButton = new QRadioButton(tr("&Female"));
    sex_maleButton->setChecked(true);
    sex_layout->addWidget(sex_maleButton);
    sex_layout->addWidget(sex_femaleButton);
    sex_group->setLayout(sex_layout);
    bv_layout->addWidget(sex_group);
    connect(sex_maleButton, &QRadioButton::toggled, [=](bool toggled) {this->m_patientIsMale=toggled;
    this->updateBloodVolume(); });
    connect(sex_femaleButton, &QRadioButton::toggled, [=](bool toggled) {this->m_patientIsMale=!toggled;
        this->updateBloodVolume(); });

    auto bv_heightBox = addSpinBox(bv_group, "Patient height:");
    bv_heightBox->setMinimum(100.0);
    bv_heightBox->setMaximum(200.0);
    bv_heightBox->setValue(180.0);
    bv_heightBox->setSuffix(" cm");
    auto bv_weightBox = addSpinBox(bv_group, "Patient weight:");
    bv_weightBox->setMinimum(40.0);
    bv_weightBox->setMaximum(150.0);
    bv_weightBox->setValue(80.0);
    bv_weightBox->setSuffix(" kg");

    m_bloodVolumeSpinBox = addSpinBox(bv_group, "Blood volume for patient:");
    m_bloodVolumeSpinBox->setSuffix(" ml");
    m_bloodVolumeSpinBox->setMinimum(2000.0);
    m_bloodVolumeSpinBox->setMaximum(10000.0);
    m_bloodVolumeSpinBox->setEnabled(false);

    m_cardiacOutputSpinBox = addSpinBox(bv_group, "Cardiac output for patient:");
    m_cardiacOutputSpinBox->setSuffix(" ml/min");
    m_cardiacOutputSpinBox->setMinimum(2000.0);
    m_cardiacOutputSpinBox->setMaximum(10000.0);
    m_cardiacOutputSpinBox->setEnabled(false);

    connect(bv_calculatorBox, &QCheckBox::stateChanged, [=](const int state) {
        bool use_calculator = state == Qt::Checked;
        bv_heightBox->setEnabled(use_calculator);
        bv_weightBox->setEnabled(use_calculator);
        m_bloodVolumeSpinBox->setEnabled(!use_calculator);
        m_cardiacOutputSpinBox->setEnabled(!use_calculator);
        sex_group->setEnabled(use_calculator);
        this->m_useBloodVolumeCalculator = use_calculator;
        this->updateBloodVolume();
    });

    connect(bv_heightBox, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double height) {
        this->m_patientHeight = height;
        this->updateBloodVolume();
    });
    connect(bv_weightBox, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double weight) {
        this->m_patientWeight = weight;
        this->updateBloodVolume();
    });

    connect(m_bloodVolumeSpinBox, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double bv) {
        this->m_patientBloodVolume = bv;
        this->updateBloodVolume();
    });
    connect(m_cardiacOutputSpinBox, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double co) {
        this->m_patientCardiacOutput = co;
        this->updateBloodVolume();
    });
}

void SettingsWidget::updateBloodVolume()
{
    if (m_useBloodVolumeCalculator) {
        if (m_patientIsMale) {
            m_patientBloodVolume = 33.164 * std::pow(0.39370079 * m_patientHeight, 0.725) * std::pow(2.20462262 * m_patientWeight, 0.425) - 1.229;
        } else {
            m_patientBloodVolume = 34.85 * std::pow(0.39370079 * m_patientHeight, 0.725) * std::pow(2.20462262 * m_patientWeight, 0.425) - 1.954;
        }
        m_patientCardiacOutput = 36.36 * std::pow(0.39370079 * m_patientHeight, 0.725) * std::pow(2.20462262 * m_patientWeight, 0.425);

        m_bloodVolumeSpinBox->setEnabled(true);
        m_bloodVolumeSpinBox->setValue(m_patientBloodVolume);
        m_bloodVolumeSpinBox->setEnabled(false);
        m_cardiacOutputSpinBox->setEnabled(true);
        m_cardiacOutputSpinBox->setValue(m_patientCardiacOutput);
        m_cardiacOutputSpinBox->setEnabled(false);
    }
}

QDoubleSpinBox* SettingsWidget::addSpinBox(QWidget* parent, const QString& description)
{
    auto subLayout = new QHBoxLayout();
    auto sbox = new QDoubleSpinBox(parent);
    auto label = new QLabel(description, parent);
    subLayout->addWidget(label);
    subLayout->addWidget(sbox);
    parent->layout()->addItem(subLayout);
    return sbox;
}
