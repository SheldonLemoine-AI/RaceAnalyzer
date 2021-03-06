/*
 * gpsView.h
 *
 *  Created on: 2009-07-22
 *      Author: brent
 */

#ifndef GPSVIEW_H_
#define GPSVIEW_H_
#include <wx/wx.h>
#include <wx/dynarray.h>
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
#include <vector>
#include <algorithm>

using std::vector;

#define POINT_SCALING	100000

class GPSPoint{
public:
	GPSPoint() :
		x(0),y(0),z(0),l(0),r(0)
		{}
	GPSPoint(double newX, double newY, double newZ, double newL, double newR) :
		x(newX), y(newY), z(newZ), l(newL), r(newR)
		{}
	double x;
	double y;
	double z;
	double l;
	double r;
};

WX_DECLARE_OBJARRAY(GPSPoint,GPSPoints);
WX_DEFINE_SORTED_ARRAY(double *, SortedPoints);

class GPSView : public wxWindow
{
public:

	 GPSView( wxWindow *parent, wxWindowID id = wxID_ANY,
	        const wxPoint& pos = wxDefaultPosition,
	        const wxSize& size = wxDefaultSize,
	        long style = 0, const wxString& name = _T("GPSView") );

	    ~GPSView();
	    void OnSize(wxSizeEvent& event);
	    inline double SCALE(double point, double min, double max, double currentSize, double zoom);
	    void OnPaint(wxPaintEvent& event);
	    void ClearGPSPoints();
	    void AddGPSPoints(GPSPoints &point);
	    void OnEraseBackground(wxEraseEvent& event);
	    void OnEnterWindow( wxMouseEvent& event );
	    void OnMouseWheel( wxMouseEvent& event );
	    void SetMarker(GPSPoint &p);
	    GPSPoint GetMarker();
private:

	    GPSPoints		m_gpsPoints;
	    GPSPoint		m_marker;
	    double			m_minX;
	    double			m_maxX;
	    double			m_minY;
	    double			m_maxY;
	    double 			m_zoom;

		int				_currentWidth;
		int				_currentHeight;
		wxBitmap 		*_memBitmap;
	    DECLARE_EVENT_TABLE()

};



#endif /* GPSVIEW_H_ */
