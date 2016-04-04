#pragma 

template <typename T>
QList<QPointF> convertv(T begin,T end)
{
	QList<QPointF> pts2;
	while (begin != end)
	{
		pts2 << QPointF(begin->x(),begin->y());
		++begin;
	}
	return pts2;
}

template <typename T>
QList<QPointF> convert(T begin,T end){
	QList<QPointF> pts2;
	while (begin != end)
	{
		pts2 << QPointF(begin->first.x(),begin->first.y());
		++begin;
	}
	return pts2;
}

template <typename T>
QList<QPointF> convert(T pt){
	QList<QPointF> pts2;
	pts2 << QPointF(pt.x(),pt.y());
	return pts2;
}

#define AW 	QApplication app(argc,argv);\
			tt3::ShowPointWidget w;\
			w.set_window(-5,5,-5,5);\
			w.show();
#define APP AW

#define AddPoints(pts,color,size)	w.Show_point_layer()->add_points(pts,color,size);\
						w.redraw();
#define AP AddPoints

#define EXEC app.exec();

#define CVV int argc,char ** argv