
#include "plotwidget.h"

#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>

PlotWidget::PlotWidget(const QMap<int, QString>& organs, QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout;
    setLayout(layout);

    m_chart = new QtCharts::QChart();

    m_xAxis = new QtCharts::QValueAxis();
    //m_xAxis->setFormat("mm:ss.z");
    m_xAxis->setTitleText(tr("Time [minutes]"));
    m_yAxis = new QtCharts::QValueAxis();
    m_yAxis->setTitleText(tr("Concentration [mg/ml Iodine]"));
    m_chart->addAxis(m_yAxis, Qt::AlignLeft);
    m_chart->addAxis(m_xAxis, Qt::AlignBottom);

    m_chartView = new QtCharts::QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(m_chartView);

    setAvailableOrgans(organs);
}

void PlotWidget::setupListView()
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
}

void PlotWidget::setAvailableOrgans(const QMap<int, QString>& organs)
{
    m_organs = organs;
    setupListView();
}

void PlotWidget::setSeries(SeriesPtr series)
{
    if (!series)
        return;
    m_chart->removeAllSeries();

    int size = series->data.size();

    for (int i = 0; i < series->data.size(); ++i) {
        const auto& s = series->data[i];
        const auto& name = series->names[i];
        auto c = new QtCharts::QSplineSeries();
        c->append(s);
        c->setName(name);
        m_chart->addSeries(c);
        c->attachAxis(m_xAxis);
        c->attachAxis(m_yAxis);
    }

    m_xAxis->setRange(series->xMin, series->xMax);
    m_yAxis->setRange(series->yMin, series->yMax);

    m_yAxis->applyNiceNumbers();
    m_xAxis->applyNiceNumbers();
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
