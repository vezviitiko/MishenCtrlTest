#include "MishenCtrl.h"

#include <MishenCtrlTest/MishenCtrlTest.h>

using namespace Upp;
extern MishenCtrlTest *mwp;


MishenCtrl::MishenCtrl():
	history_length(25)
{
	max_meters = 10;
	showCoordinates = false;
	SetTimeCallback(-10,THISBACK(Update));
}

void MishenCtrl::PutPoint( double _x, double _y,int n ){
//	DUMP(n);

//	DUMP(points0.GetCount());

//	DUMP(points1.GetCount());

//	DUMP(points2.GetCount());

	if (n==0){
		PointDbl &point = points0.Add();
		point.x  = _x;
		point.y  = _y;
	} else if (n==1) {
		PointDbl &point = points1.Add();
		point.x  = _x;
		point.y  = _y;
	} else if (n==2) {
		PointDbl &point = points2.Add();
		point.x  = _x;
		point.y  = _y;
	}
	if (
		points0.GetCount() >= history_length+1 ||
		points1.GetCount() >= history_length+1 ||
		points2.GetCount() >= history_length+1
		)
	{
		if ( points0.GetCount() >0 ) points0.Remove(0);
		if ( points1.GetCount() >0 ) points1.Remove(0);
		if ( points2.GetCount() >0 ) points2.Remove(0);
	}
	Refresh();
}

void MishenCtrl::Update(){
	Refresh();
}
void MishenCtrl::Paint(Draw &w){
	const double zoom_steps = 20.0;
	const double unzoom_steps = 100.0;
	
	if (max_meters_new < 5.0) max_meters_new = 5.0;
	double diff = max_meters_new-max_meters;
	if (diff > 0.0) {
		max_meters += diff/zoom_steps;
	} else {
		max_meters += diff/unzoom_steps;
	}
	
	if (abs(diff) > 0.1 ){
	//	mwp->WriteLog(Format("current=%2.2f, diff=%2.2f , new=%2.2f",max_meters,diff,max_meters_new));

	}

	Color color_setka = Color(9, 222, 222);
	
	Size sz = GetSize();
	int min_side = min(sz.cx,sz.cy);
	sz.cx = min_side;
	sz.cy = min_side;

	w.DrawRect(sz,White());
	//Расчет значения шага сетки

	int step = 1;
	int step0 = 1;
	int state = 0;
	if (max_meters > 10) step = 2;
	if (max_meters > 20) step = 5;
	if (max_meters > 50) step = 10;

	int steps = (max_meters+1)/step;

	while (steps > 10) {
		state +=1;
		if (state%3==0){
			step0 *= 10;
			step = step0;
		} else if (state%3==1) {
			step = step0*2;
		} else if (state%3==2) {
			step = step0*5;
		}
		steps = (max_meters+1)/step;
	}
	//mwp->WriteLog(Format("  step=%2.2f steps=%2.2f state=%2.2f\n",step,steps,state));

	// Отрисовка кругов сетки мишени, осей, надписей

	int64 i = steps * step;
	bool hor_axe_angle45 = false;
	while (i > 0){
		if (i < max_meters ) {
			// Отрисовка кругов сетки мишени

			int dx = i * (sz.cx/2) / max_meters;
			int dy = i * (sz.cy/2) / max_meters;
			w.DrawEllipse(	sz.cx/2-dx,
							sz.cy/2-dy,
							dx*2,
							dy*2,
							White(),1,color_setka);
			String label;
			if (i < 1000){
				label = Format("%d",i);
			}else{
				label = Format("%dкм",i/1000);
			}
			// Отрисовка надписей

			//mwp->WriteLog(Format("  i=%2.2f",i));

			Font fnt = StdFont(12);
			Size sztx = GetTextSize(label,fnt);
			if (sztx.cx*1.2 > (step*sz.cx/2) / max_meters) hor_axe_angle45 = true;
			w.DrawText(sz.cx/2+1,sz.cy/2-dy-6,label,fnt);
			if (!hor_axe_angle45){
				w.DrawText(sz.cx/2+dy-(sztx.cx/2),sz.cy/2-2,label,fnt);
			} else {
				w.DrawText(sz.cx/2+dy-(sztx.cx/2),sz.cy/2+sztx.cx/1.5,450,label,fnt);
			}
		}
		i-=step;
	}
	// Отрисовка осей, надписей

	w.DrawLine(sz.cx/2,0,sz.cx/2,sz.cy,2,color_setka);
	w.DrawLine(sz.cx/2+1,1,sz.cx/2+6,10,2,color_setka);
	w.DrawLine(sz.cx/2,1,sz.cx/2-5,10,2,color_setka);
	
	w.DrawLine(sz.cx,sz.cy/2,0,sz.cy/2,2,color_setka);
	w.DrawLine(sz.cx-1,sz.cy/2+1,sz.cx-10,sz.cy/2+6,2,color_setka);
	w.DrawLine(sz.cx-1,sz.cy/2,sz.cx-10,sz.cy/2-5,2,color_setka);


	max_meters_new = 5;
	LOG("==================================");

	for(int i = 0; i < points2.GetCount(); i++ ){
		int colordist = ( points2.GetCount() - i - 1 ) * 2 + 196 ;
		//LOG(Format("i=%d, points.GetCount()= %d, colordist=%d",i,points.GetCount(),colordist));

		if (colordist < 0) colordist = 0;
		int px,py;
		if (colordist >= 0  && colordist <=255) {
			Color colorb = Color(colordist,colordist,colordist);
			Color colorb2 = Color(colordist-50,colordist-50,colordist-50);
//			LOG(Format("i=%d, points.GetCount()= %d, colordist=%d, color=%s",i,points.GetCount(),colordist,AsString(colorb)));

			
			px = points2[i].x * (sz.cx/2)  / max_meters +  (sz.cx/2);
			py = sz.cy -  (sz.cy/2) - (points2[i].y * (sz.cy/2)  / max_meters);
			//w.DrawRect(px-7,py-7,13,13,colorb);

			w.DrawEllipse(px-7,py-7,13,13,colorb,0,White());
			w.DrawRect(px-3,py-3,5,5,colorb2);
			
			
			
			if (abs(points2[i].x) > max_meters_new)  max_meters_new = abs(points2[i].x );
			if (abs(points2[i].y) > max_meters_new)  max_meters_new = abs(points2[i].y );
		} else {
			//break;

		}
		
		if (showCoordinates && i == points2.GetCount()-1) {
			w.DrawText(px+3,py-6,Format("(%2.2f,%2.2f)",points2[i].x,points2[i].y),StdFont(9));
		}
	}
	for(int i = 0; i < points1.GetCount(); i++ ){
		Color colortarget = Color(0,0,255);
		int di = 0;
		if (points1.GetCount() < history_length) di = history_length - points1.GetCount();
		double percent = (double)(i+di) / (double)history_length;
		if (percent > 1.0) percent = 1.0;
		Color colorb = Blend( White(), colortarget, percent * 255.0);

		LOG(Format("i=%d, points1.GetCount()= %d, percent=%f color=%s",i,points1.GetCount(),percent,AsString(colorb)));

		int px,py;
		px = points1[i].x * (sz.cx/2)  / max_meters +  (sz.cx/2);
		py = sz.cy -  (sz.cy/2) - (points1[i].y * (sz.cy/2)  / max_meters);
		w.DrawRect(px-3,py-3,5,5,colorb);
		
		if (abs(points1[i].x) > max_meters_new)  max_meters_new = abs(points1[i].x );
		if (abs(points1[i].y) > max_meters_new)  max_meters_new = abs(points1[i].y );
		
		if (showCoordinates && i == points1.GetCount()-1) {
			w.DrawText(px+3,py-6,Format("(%2.2f,%2.2f)",points1[i].x,points1[i].y),StdFont(9));
		}
	}

//============================================================//

//=== Вывод нашего решения	==================================//

//============================================================//

	for(int i = 0; i < points0.GetCount(); i++ ){
		Color colortarget = Color(0,120,0);
		int di = 0;
		if (points0.GetCount() < history_length) di = history_length - points0.GetCount();
		double percent = (double)(i+di) / (double)history_length;
		if (percent > 1.0) percent = 1.0;
		Color colorb = Blend( White(), colortarget, percent * 255.0);

		LOG(Format("i=%d, points0.GetCount()= %d, percent=%f color=%s",i,points0.GetCount(),percent,AsString(colorb)));

		int px,py;
		px = points0[i].x * (sz.cx/2)  / max_meters +  (sz.cx/2);
		py = sz.cy -  (sz.cy/2) - (points0[i].y * (sz.cy/2)  / max_meters);
		w.DrawRect(px-3,py-3,5,5,colorb);
		
		if (abs(points0[i].x) > max_meters_new)  max_meters_new = abs(points0[i].x );
		if (abs(points0[i].y) > max_meters_new)  max_meters_new = abs(points0[i].y );
		
		if (showCoordinates && i == points0.GetCount()-1) {
			w.DrawText(px+3,py-6,Format("(%2.2f,%2.2f)",points0[i].x,points0[i].y),StdFont(9));
		}
	}



	max_meters_new *= 1.1;
	//DUMP( abs(points[i].x ) );

	//DUMP( max_meters_new );

	//DUMP( max_meters );

	//mwp->WriteLog(Format("           new max= %2.2f ",));

}

