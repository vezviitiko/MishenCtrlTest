#ifndef _MishenCtrl_MishenCtrl_h
#define _MishenCtrl_MishenCtrl_h

#include <CtrlLib/CtrlLib.h>

NAMESPACE_UPP

typedef Point_<double> PointDbl;

class MishenCtrl : public ImageCtrl {
	typedef MishenCtrl CLASSNAME;
	public:
		MishenCtrl();
		void Paint( Draw &w );
		void PutPoint( double _x, double _y,int n=0 );
		void Clear() {
			points0.Clear();
			points1.Clear();
			points2.Clear();
			Refresh();
		}
		void ShowCoordinates(bool show = true) {showCoordinates = show;}
		void Update();
		
		WithDeepCopy<Array<PointDbl> > points0;
		WithDeepCopy<Array<PointDbl> > points1;
		WithDeepCopy<Array<PointDbl> > points2;
	private:
		double max_meters;
		double max_meters_prev;
		double max_meters_new;
		int history_length;
		bool showCoordinates;
};

END_UPP_NAMESPACE

#endif
