/*
 * lineChartPane.cpp
 *
 *  Created on: Jun 7, 2009
 *      Author: brent
 */
#include "lineChartPane.h"
#include "datalogData.h"

#define SCROLLBAR_RANGE 10000
#define	SCROLLBAR_THUMBSIZE 100
#define SCROLLBAR_PAGESIZE 100


LineChartPane::LineChartPane() : wxPanel()
{
	InitComponents();
}

LineChartPane::LineChartPane(wxWindow *parent,
			wxWindowID id,
			const wxPoint &pos,
			const wxSize &size,
			long style,
			const wxString &name
			)
			: wxPanel(	parent,
						id,
						pos,
						size,
						style,
						name)
{
	InitComponents();
}

LineChartPane::~LineChartPane(){

}

void LineChartPane::ConfigureChart(DatalogChannelSelectionSet *selectionSet){

	LineChart *lineChart = GetLineChart();
	AppOptions *appOptions = m_chartParams.appOptions;
	ChartColors &chartColors = appOptions->GetChartColors();
	size_t maxColors = chartColors.Count();
	size_t currentColor = 0;


	lineChart->ClearAllSeries();

	size_t selCount = selectionSet->Count();
	for (size_t selIndex = 0; selIndex < selCount; selIndex++){
		DatalogChannelSelection &sel = selectionSet->Item(selIndex);

		wxString channelName = sel.channelName;
		ViewChannel viewChannel(sel.datalogId, sel.channelName);

		DatalogChannelType channelType = appOptions->GetChannelTypeForChannel(viewChannel);

		Range *range = new Range(channelType.minValue, channelType.maxValue, channelType.precision, channelType.unitsLabel);
		int newRangeId = lineChart->AddRange(range);

		Series *series = new Series(0, newRangeId, 0, viewChannel.ToString(), chartColors[currentColor], channelType.precision);
		currentColor =  currentColor < maxColors - 1 ? currentColor + 1 : 0;
		lineChart->AddSeries(viewChannel.ToString(), series);
	}
}

void LineChartPane::SetChartParams(ChartParams params){
	m_chartParams = params;
}

void LineChartPane::SetBufferSize(ViewChannels &channels, size_t size){

	ViewChannels enabledChannels;
	for (int i = 0; i < channels.Count(); i++){
		ViewChannel &channel = channels[i];
		Series *series = m_lineChart->GetSeries(channel.ToString());
		if (NULL != series){
			enabledChannels.Add(channel);
			series->SetBufferSize(size);
		}
	}

	wxCommandEvent addEvent(REQUEST_DATALOG_DATA_EVENT, ID_REQUEST_DATALOG_DATA);
	RequestDatalogRangeParams *params = new RequestDatalogRangeParams(this, enabledChannels, 0, size - 1);
	addEvent.SetClientData(params);
	GetParent()->GetEventHandler()->AddPendingEvent(addEvent);
}

void LineChartPane::UpdateValueRange(ViewDataHistoryArray &historyArray, size_t fromIndex, size_t toIndex){

	for (size_t i = 0; i < historyArray.size(); i++){
		ViewDataHistory &history = historyArray[i];
		Series *series = m_lineChart->GetSeries(history.channel.ToString());
		if (NULL != series){
			for (size_t i = fromIndex; i < toIndex; i++){
				series->SetValueAt(i, history.values[i]);
			}
			m_lineChart->Refresh();
		}
	}
}

void LineChartPane::UpdateValue(ViewChannel &channel, size_t index, double value){

	Series *series = m_lineChart->GetSeries(channel.ToString());
	if (NULL != series){
		size_t len = m_lineChart->GetMaxSeriesBufferSize();
		m_lineChart->SetMarkerIndex(index);
		int center = m_lineChart->GetChartWidth() / 2;
		double adjustedIndex = (double)index - center;
		adjustedIndex = adjustedIndex >= 0 ? adjustedIndex : 0;
		double factor = adjustedIndex / (double)len;
		double thumbPos = SCROLLBAR_RANGE * factor;
		m_scrollBar->SetThumbPosition(thumbPos);
		ScrollLineChart(thumbPos);
	}
}

void LineChartPane::ScrollLineChart(int thumbPosition){
	double pct = ((double)thumbPosition) / SCROLLBAR_RANGE;
	m_lineChart->SetViewOffsetFactor(pct);
}

void LineChartPane::SetOffset(int offset){

}

void LineChartPane::InitComponents(){

	wxFlexGridSizer *sizer = new wxFlexGridSizer(2,1,0,0);
	sizer->AddGrowableCol(0);
	sizer->AddGrowableRow(0);

	m_lineChart = new LineChart(this);
	m_lineChart->SetBackgroundColour(*wxBLACK);
	m_lineChart->ShowScale(false);
	m_scrollBar = new wxScrollBar(this,ID_LOGVIEWER_SCROLL);

	m_scrollBar->SetScrollbar(0,SCROLLBAR_THUMBSIZE, SCROLLBAR_RANGE, SCROLLBAR_PAGESIZE,false);

	sizer->Add(m_lineChart,1,wxEXPAND);
	sizer->Add(m_scrollBar,1,wxEXPAND);
	this->SetSizer(sizer);

}

LineChart * LineChartPane::GetLineChart(){
	return m_lineChart;
}

void LineChartPane::OnScroll(wxScrollEvent &event){
	ScrollLineChart(m_scrollBar->GetThumbPosition());
}

BEGIN_EVENT_TABLE ( LineChartPane, wxPanel )
	EVT_COMMAND_SCROLL (ID_LOGVIEWER_SCROLL,LineChartPane::OnScroll)
END_EVENT_TABLE()
