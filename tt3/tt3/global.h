#pragma 


// template <typename T>
// QList<QPointF> convertv(T begin,T end)
// {
// 	QList<QPointF> pts2;
// 	while (begin != end)
// 	{
// 		pts2 << QPointF(begin->x(),begin->y());
// 		++begin;
// 	}
// 	return pts2;
// }

template <typename T>
QList<QPointF> convert_pair_map(T begin,T end){
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

template<typename Iter>
QList<QPointF> convert_list_vector(Iter begin,Iter end){
	QList<QPointF> res;
	while (begin != end)
	{
		res << QPointF(begin->x(),begin->y());
		++begin;
	}
	return res;
}

#define AW 	QApplication app(argc,argv);\
			layerwidget::Qt_default_widget w;\
			w.set_window(-5,5,-5,5);\
			w.show();\
			w.redraw();
#define APP AW

#define SW(x1,x2,y1,y2) w.set_window(x1,x2,y1,y2);

#define AddPoints(pts,color,size)	w.get_geometry_layer()->add_points(pts,color,size);\
						w.redraw();

#define AddPolygon(pts,border_c,border_w,fill_c) Style style;\
			style.border_color = border_c;\
			style.fill_color = fill_c;\
			style.border_width = border_w;\
			w.get_geometry_layer()->add_polygon(pts,style);

#define AP AddPoints

#define AddEllipse(pt,width,height) w.get_geometry_layer()->add_ellipse(pt,width,height);

#define EXEC app.exec();

#define CVV int argc,char ** argv

inline void test(){
	vector<int> vec;
	back_inserter(vec)++ = 1;
}