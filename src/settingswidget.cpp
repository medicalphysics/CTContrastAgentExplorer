#include "settingswidget.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QTimer>
#include <QVBoxLayout>

#include <cmath>

SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    auto patient_group = new QGroupBox(tr("Patient data"));
    auto patient_layout = new QVBoxLayout;
    patient_group->setLayout(patient_layout);
    mainLayout->addWidget(patient_group);

    //blood volume
    auto bv_group = new QGroupBox(tr("Use calculator"), this);
    bv_group->setCheckable(true);
    bv_group->setChecked(true);
    auto bv_layout = new QVBoxLayout;
    auto bv_widget = new CalculatorSettingsWidget;
    bv_group->setLayout(bv_layout);
    bv_layout->addWidget(bv_widget);
    patient_layout->addWidget(bv_group);
    connect(bv_widget, &CalculatorSettingsWidget::bloodVolumeChanged, [=](double bv) { emit this->bloodVolumeChanged(bv); });
    connect(bv_widget, &CalculatorSettingsWidget::renalClearenceChanged, [=](double bv) { emit this->renalClearenceChanged(bv); });
    connect(bv_widget, &CalculatorSettingsWidget::cardiacOutputChanged, [=](double bv) { emit this->cardiacOutputChanged(bv); });

    auto r_widget = new RawSettingsWidget();
    patient_layout->addWidget(r_widget);
    connect(bv_group, &QGroupBox::toggled, r_widget, &RawSettingsWidget::setUseCalculator);
    connect(bv_group, &QGroupBox::toggled, [=](auto togg) {
        if (togg)
            bv_widget->calculate();
    });
    connect(bv_widget, &CalculatorSettingsWidget::bloodVolumeChanged, r_widget, &RawSettingsWidget::setBloodVolume);
    connect(bv_widget, &CalculatorSettingsWidget::cardiacOutputChanged, r_widget, &RawSettingsWidget::setCardiacOutput);
    connect(bv_widget, &CalculatorSettingsWidget::renalClearenceChanged, r_widget, &RawSettingsWidget::setRenalClearence);

    connect(r_widget, &RawSettingsWidget::bloodVolumeChanged, [=](double bv) { emit this->bloodVolumeChanged(bv); });
    connect(r_widget, &RawSettingsWidget::renalClearenceChanged, [=](double bv) { emit this->renalClearenceChanged(bv); });
    connect(r_widget, &RawSettingsWidget::cardiacOutputChanged, [=](double bv) { emit this->cardiacOutputChanged(bv); });

    auto contrast_group = new QGroupBox(tr("Contrast injection"));
    auto contrast_layout = new QVBoxLayout;
    contrast_group->setLayout(contrast_layout);
    mainLayout->addWidget(contrast_group);
    auto contrast_widget = new ContrastSettingsWidget();
    contrast_layout->addWidget(contrast_widget);
    connect(contrast_widget, &ContrastSettingsWidget::volumeChanged, [=](const double v) { emit this->contrastInjectionVolumeChanged(v); });
    connect(contrast_widget, &ContrastSettingsWidget::timeChanged, [=](const double v) { emit this->contrastInjectionTimeChanged(v); });
    connect(contrast_widget, &ContrastSettingsWidget::concentrationChanged, [=](const double v) { emit this->contrastInjectionConcentrationChanged(v); });

    mainLayout->addStretch();

    //initializatio
    emit bv_group->toggled(bv_group->isChecked());
    bv_widget->calculate();
}

CalculatorSettingsWidget::CalculatorSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout;
    setLayout(layout);

    //adding sex button
    auto sex_group = new QGroupBox(tr("Patient sex"));
    auto sex_layout = new QHBoxLayout;
    auto sex_maleButton = new QRadioButton(tr("&Male"));
    auto sex_femaleButton = new QRadioButton(tr("&Female"));
    sex_maleButton->setChecked(true);
    sex_layout->addWidget(sex_maleButton);
    sex_layout->addWidget(sex_femaleButton);
    sex_group->setLayout(sex_layout);
    layout->addWidget(sex_group);
    connect(sex_maleButton, &QRadioButton::toggled, [=](bool toggled) {
        this->m_sexIsMale=toggled;
        this->calculate(); });
    connect(sex_femaleButton, &QRadioButton::toggled, [=](bool toggled) {
        this->m_sexIsMale=!toggled;
        this->calculate(); });

    //adding weight
    auto weight_layout = new QHBoxLayout;
    auto weight_label = new QLabel(tr("Patient weight"));
    auto weight_spin = new QDoubleSpinBox();
    weight_spin->setMinimum(40);
    weight_spin->setMaximum(150);
    weight_spin->setValue(m_patientWeight);
    weight_spin->setSuffix("kg");
    weight_spin->setDecimals(0);
    weight_layout->addWidget(weight_label);
    weight_layout->addWidget(weight_spin);
    layout->addLayout(weight_layout);
    connect(weight_spin, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double weight) {
        this->m_patientWeight = weight;
        this->calculate();
    });

    //adding height
    auto height_layout = new QHBoxLayout;
    auto height_label = new QLabel(tr("Patient height"));
    auto height_spin = new QDoubleSpinBox();
    height_spin->setMinimum(150);
    height_spin->setMaximum(210);
    height_spin->setValue(m_patientHeight);
    height_spin->setSuffix("cm");
    height_spin->setDecimals(0);
    height_layout->addWidget(height_label);
    height_layout->addWidget(height_spin);
    layout->addLayout(height_layout);
    connect(height_spin, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double height) {
        this->m_patientHeight = height;
        this->calculate();
    });

    //adding age
    auto age_layout = new QHBoxLayout;
    auto age_label = new QLabel(tr("Patient age"));
    auto age_spin = new QSpinBox();
    age_spin->setMinimum(12);
    age_spin->setMaximum(150);
    age_spin->setValue(m_patientAge);
    age_spin->setSuffix(tr("yrs"));
    age_layout->addWidget(age_label);
    age_layout->addWidget(age_spin);
    layout->addLayout(age_layout);
    connect(age_spin, QOverload<const int>::of(&QSpinBox::valueChanged), [=](const int age) {
        this->m_patientAge = age;
        this->calculate();
    });

    //adding creatinine
    auto c_layout = new QHBoxLayout;
    auto c_label = new QLabel(tr("Patient serum creatinine"));
    auto c_spin = new QDoubleSpinBox();
    c_spin->setMinimum(10);
    c_spin->setMaximum(500);
    c_spin->setValue(m_patientCreatinine);
    c_spin->setSuffix(tr("uMol/l"));
    c_spin->setDecimals(0);
    c_layout->addWidget(c_label);
    c_layout->addWidget(c_spin);
    layout->addLayout(c_layout);
    connect(c_spin, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double c) {
        this->m_patientCreatinine = c;
        this->calculate();
    });

    QTimer::singleShot(1, [=]() { this->calculate(); });
}

void CalculatorSettingsWidget::calculate()
{
    const double a = m_sexIsMale ? 23.6 : 24.8;
    const double b = m_sexIsMale ? 1229 : 1954;
    const double BV = a * std::pow(m_patientHeight, 0.725) * std::pow(m_patientWeight, 0.425) - b;

    emit bloodVolumeChanged(BV);

    const double ref = 25.3 * std::pow(m_patientHeight, 0.725) * std::pow(m_patientWeight , 0.425);
    const auto CO = ref * (1.0 - (m_patientAge - 30.0) * 1.01 / 100.0);
    emit cardiacOutputChanged(CO);

    const double Q = m_sexIsMale ? 1.23 : 1.04;
    const double RC = Q * (140.0 - m_patientAge) * m_patientWeight / m_patientCreatinine;
    emit renalClearenceChanged(RC);
}

RawSettingsWidget::RawSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout;
    setLayout(layout);

    //adding bv
    auto bv_layout = new QHBoxLayout;
    auto bv_label = new QLabel(tr("Patient blood volume"));
    bv_spin = new QDoubleSpinBox();
    bv_spin->setMinimum(100);
    bv_spin->setMaximum(10000);
    bv_spin->setSuffix("ml");
    bv_layout->addWidget(bv_label);
    bv_layout->addWidget(bv_spin);
    layout->addLayout(bv_layout);
    connect(bv_spin, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double bv) {
        emit this->bloodVolumeChanged(bv);
    });

    //adding co
    auto co_layout = new QHBoxLayout;
    auto co_label = new QLabel(tr("Patient cardiac output"));
    co_spin = new QDoubleSpinBox();
    co_spin->setMinimum(100);
    co_spin->setMaximum(10000);
    co_spin->setSuffix("ml/min");
    co_layout->addWidget(co_label);
    co_layout->addWidget(co_spin);
    layout->addLayout(co_layout);
    connect(co_spin, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double co) {
        emit this->cardiacOutputChanged(co);
    });

    //adding rc
    auto rc_layout = new QHBoxLayout;
    auto rc_label = new QLabel(tr("Patient renal clearence"));
    rc_spin = new QDoubleSpinBox();
    rc_spin->setMinimum(0.1);
    rc_spin->setMaximum(1000);
    rc_spin->setSuffix("ml/min");
    rc_layout->addWidget(rc_label);
    rc_layout->addWidget(rc_spin);
    layout->addLayout(rc_layout);
    connect(rc_spin, QOverload<const double>::of(&QDoubleSpinBox::valueChanged), [=](const double co) {
        emit this->renalClearenceChanged(co);
    });
}

void RawSettingsWidget::setUseCalculator(bool c)
{
    m_useCalculator = c;
    bv_spin->setDisabled(c);
    co_spin->setDisabled(c);
    rc_spin->setDisabled(c);
}

void RawSettingsWidget::setBloodVolume(double bv)
{
    bv_spin->blockSignals(true);
    bv_spin->setEnabled(true);
    bv_spin->setValue(bv);
    bv_spin->setDisabled(m_useCalculator);
    bv_spin->blockSignals(false);
}
void RawSettingsWidget::setCardiacOutput(double bv)
{
    co_spin->blockSignals(true);
    co_spin->setEnabled(true);
    co_spin->setValue(bv);
    co_spin->setDisabled(m_useCalculator);
    co_spin->blockSignals(false);
}
void RawSettingsWidget::setRenalClearence(double bv)
{
    rc_spin->blockSignals(true);
    rc_spin->setEnabled(true);
    rc_spin->setValue(bv);
    rc_spin->setDisabled(m_useCalculator);
    rc_spin->blockSignals(false);
}

ContrastSettingsWidget::ContrastSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    auto main_layout = new QVBoxLayout;
    setLayout(main_layout);

    QDoubleSpinBox* spins[4];
    QString labels[4] = {
        tr("Injection volume"),
        tr("Injection time"),
        tr("Injection rate"),
        tr("Contrast agent strenght")
    };

    for (int i = 0; i < 4; ++i) {
        spins[i] = new QDoubleSpinBox();
        auto label = new QLabel(labels[i]);
        auto lay = new QHBoxLayout;
        lay->addWidget(label);
        lay->addWidget(spins[i]);
        main_layout->addLayout(lay);
        spins[i]->setMinimum(0.0);
        spins[i]->setMaximum(800.0);
    }

    spins[0]->setSuffix("ml");
    spins[1]->setSuffix("s");
    spins[2]->setSuffix("ml/s");
    spins[3]->setSuffix("mg/ml Iodine");

    spins[3]->setDecimals(0);

    connect(spins[0], QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](const double v) { emit this->volumeChanged(v); });
    connect(spins[1], QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](const double time) {
        const double v = spins[0]->value();
        const double rate = v / time;
        spins[2]->blockSignals(true);
        spins[2]->setValue(rate);
        spins[2]->blockSignals(false);
        emit this->timeChanged(time/60.0);
    });
    connect(spins[3], QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](const double v) { emit this->concentrationChanged(v); });
    connect(spins[2], QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](const double rate) {
        const double v = spins[0]->value();
        const double time = v / rate;
        spins[1]->blockSignals(true);
        spins[1]->setValue(time);
        spins[1]->blockSignals(false);
        emit this->timeChanged(time / 60.0);
    });

    QTimer::singleShot(1, [=]() {
        spins[0]->setValue(125);
        spins[2]->setValue(2.5);
        spins[3]->setValue(320);
    });
}
