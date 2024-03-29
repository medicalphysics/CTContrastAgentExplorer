
#include "plotwidget.h"

#include <QLegendMarker>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>

PlotWidget::PlotWidget(const QMap<int, QString>& organs, QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    auto chartLayout = new QVBoxLayout;
    chartLayout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(chartLayout);

    m_chart = new QChart();
    m_chart->setAnimationOptions(QChart::NoAnimation);

    m_xAxis = new QValueAxis();
    m_xAxis->setTitleText(tr("Time [seconds]"));
    m_yAxis = new QValueAxis();
    m_yAxis->setTitleText(tr("Concentration [mg/ml Iodine]"));
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);
    m_chart->addAxis(m_xAxis, Qt::AlignBottom);

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    chartLayout->addWidget(m_chartView);

    auto buttonslayout = new QHBoxLayout;
    auto time_layout = new QHBoxLayout;
    auto time_label = new QLabel(tr("Total time"));
    auto time_spin = new QDoubleSpinBox();
    time_spin->setRange(0, 60);
    time_spin->setValue(5);
    time_spin->setSuffix("min");
    time_layout->addWidget(time_label);
    time_layout->addWidget(time_spin);
    buttonslayout->addLayout(time_layout);
    connect(time_spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PlotWidget::setPlotTime);
    chartLayout->addLayout(buttonslayout);

    auto group_kvp = new QGroupBox(tr("Display CT enhancement"));
    group_kvp->setCheckable(true);
    auto group_layout = new QVBoxLayout;
    group_layout->setContentsMargins(0, 0, 0, 0);
    group_kvp->setLayout(group_layout);
    auto group_select = new QComboBox();
    for (int k = 70; k < 151; k += 10) {
        auto text = QString::number(k) + QString(" kVp");
        group_select->addItem(text, k);
    }
    group_layout->addWidget(group_select);
    buttonslayout->addWidget(group_kvp);
    buttonslayout->addStretch();
    buttonslayout->setContentsMargins(0, 0, 0, 0);
    connect(group_select, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        this->m_kVp = group_select->currentData().toInt();
        emit this->kVpChanged(this->m_kVp);
        this->setAxisShowHU(true);
    });
    connect(group_kvp, &QGroupBox::toggled, [=](bool toggled) {
        emit this->modeHUchanged(toggled);
        this->setAxisShowHU(toggled);
    });

    auto saveButton = new QPushButton("Save plot");
    buttonslayout->addWidget(saveButton);
    connect(saveButton, &QPushButton::clicked, this, &PlotWidget::savePlot);

    auto copyButton = new QPushButton("Copy data");
    buttonslayout->addWidget(copyButton);
    connect(copyButton, &QPushButton::clicked, this, &PlotWidget::copyPlotDataClipboard);

    m_organs = organs;

    QTimer::singleShot(0, [=]() { time_spin->setValue(7.0); });
    QTimer::singleShot(0, [=]() { group_select->setCurrentIndex(3); });
    QTimer::singleShot(0, [=]() { group_kvp->setChecked(false); });
}

QListView* PlotWidget::setupListView()
{
    auto mainLayout = layout();
    auto view = new QListView();
    mainLayout->addWidget(view);

    auto model = new QStandardItemModel(this);
    view->setModel(model);

    auto i = m_organs.constBegin();
    while (i != m_organs.constEnd()) {
        auto item = new QStandardItem(i.value());
        item->setCheckable(true);
        item->setCheckState(Qt::CheckState::Unchecked);
        item->setData(i.key());
        model->appendRow(item);
        ++i;
    }
    model->sort(0);
    qRegisterMetaType<QSet<int>>();
    connect(model, &QStandardItemModel::itemChanged, this, &PlotWidget::listItemChanged);
    // view->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::MinimumExpanding);
    // auto sizehint = view->sizeHintForColumn(0);
    // view->setMaximumSize(view->viewportSizeHint());
    // view->setMaximumWidth(sizehint);
    return view;
}

QString createHTMLTable(SeriesPtr series)
{
    if (!series)
        return "";
    if (series->data.size() == 0)
        return "";

    QString table;
    table += "<table><tr><td>Time [s]</td>";
    for (const auto& name : series->names)
        table += "<td>" + name + "</td>";
    table += "</tr>";

    const auto size = series->data[0].size();
    for (int i = 0; i < size; ++i) {
        table += "<tr>";
        const auto& s0 = (series->data)[0][i];
        table += "<td>" + QString::number(s0.x()) + "</td>";
        for (const auto& s : series->data) {
            table += "<td>" + QString::number(s[i].y()) + "</td>";
        }
        table += "</tr>";
    }
    return table + "</table>";
}

void PlotWidget::setSeries(SeriesPtr series)
{
    if (!series)
        return;

    m_htmlTable = createHTMLTable(series);
    m_chart->removeAllSeries();

    for (int i = 0; i < series->data.size(); ++i) {
        const auto& s = series->data[i];
        const auto& name = series->names[i];
        auto c = new QSplineSeries();
        c->append(s);
        c->setName(name);
        m_chart->addSeries(c);
        c->attachAxis(m_xAxis);
        c->attachAxis(m_yAxis);

        auto p = new QScatterSeries();
        p->append(series->peaks[i]);
        p->setPointLabelsVisible(true);
        p->setColor(c->color());
        p->setMarkerSize(1.0);
        p->setMarkerShape(QScatterSeries::MarkerShape::MarkerShapeCircle);
        p->setPointLabelsClipping(false);
        p->setPointLabelsFormat("@xPoints");
        m_chart->addSeries(p);
        p->attachAxis(m_xAxis);
        p->attachAxis(m_yAxis);

        auto legend = m_chart->legend();
        auto markers = legend->markers(p);
        if (markers.size() > 0) {
            markers[0]->setVisible(false);
        }
    }

    m_xAxis->setRange(series->xMin, m_totalTime * 60);
    m_yAxis->setRange(series->yMin, series->yMax);

    // m_xAxis->applyNiceNumbers();
    // m_yAxis->applyNiceNumbers();
}

void PlotWidget::setPlotTime(const double time)
{
    emit this->plotTimeChanged(time);
    m_totalTime = time;
}

void PlotWidget::setAxisShowHU(bool showHU)
{
    if (showHU) {
        QString txt = tr("Enhancement @ ") + QString::number(m_kVp) + "kVp [HU]";
        m_yAxis->setTitleText(txt);
    } else {
        m_yAxis->setTitleText(tr("Concentration [mg/ml Iodine]"));
    }
}

void PlotWidget::setkVp(int kvp)
{
    m_kVp = kvp;
    setAxisShowHU(true);
}

void PlotWidget::savePlot()
{
    if (m_chartView) {

        auto path = QFileDialog::getSaveFileName(this, "Save image", "untitled.png", "Images (*.png)");
        const auto dpr = m_chartView->devicePixelRatioF();
        QPixmap pixmap(m_chartView->width() * dpr, m_chartView->height() * dpr);
        pixmap.setDevicePixelRatio(dpr);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
        m_chartView->render(&painter);
        pixmap.save(path);
    }
}

void PlotWidget::copyPlotDataClipboard()
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    auto mimeData = new QMimeData;
    mimeData->setHtml(m_htmlTable);
    clipboard->setMimeData(mimeData);
}

void PlotWidget::listItemChanged(QStandardItem* item)
{
    const auto id = item->data().toInt();
    if (item->checkState() == Qt::CheckState::Checked) {
        m_checkedOrgans.insert(id);
    } else {
        m_checkedOrgans.remove(id);
    }
    emit requestSeries(m_checkedOrgans);
}
