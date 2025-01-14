
#include <iostream>
#include <filesystem>
#include <map>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "bouncy.h"
#include "bouncy.hpp"
#include "camera.hpp"
#include "processing.hpp"

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

#define MAX_COUNT 500
#define CASCADE_DIR "gdnative/bouncy/cascades/"

/* Map of CascadeClassifiers; allows easier access and better organization */
static map<string, CascadeClassifier> CascadeList;

static Mat camera, working1, working2;
static Mat previous;


int processing_initialize()
{
	/* List cascade directory */
	for (const auto & entry : fs::directory_iterator(CASCADE_DIR)) {
		string name = get_filename(entry.path());
		CascadeList[name] = CascadeClassifier();

		if (!entry.is_regular_file()) { continue; }
		else if (get_extension(entry.path()) != "xml") { continue; }

		/* Loading file */
		if (!CascadeList[name].load(entry.path())) {
			b_fprintln(stderr, "Cannot load \"%s\" cascade file", name.c_str());
		}
	}

	return 1;
}

/* Object detection */
region processing_detect_object(void *arg, const char *name)
{
	SharedCamera user_data = *((SharedCamera*) arg);
	CascadeClassifier cascade = CascadeList[name];

	camera = user_data->getFrame();
	cvtColor(camera, working1, COLOR_RGB2GRAY);

	vector<Rect> objects;
	region r; r.x = -1; // default value in case something goes wrong

	if (!cascade.empty()) {
		float scale_factor = (float) working1.cols / 320.0;

		resize(working1, working2, Size(320, (int)((float)working1.rows / scale_factor)));

		cascade.detectMultiScale(working2, objects,
			1.1, 3, CASCADE_FIND_BIGGEST_OBJECT | CASCADE_DO_ROUGH_SEARCH | CASCADE_SCALE_IMAGE,
			Size(40, 40), Size(100, 100)
		);

		/* FIXME: Only detects ONE instance of an object */
		if (objects.size() > 0) {
			r.x = objects[0].x * scale_factor;
			r.y = objects[0].y * scale_factor;
			r.w = objects[0].width * scale_factor;
			r.h = objects[0].height * scale_factor;
		}
	}

	return r;
}

/* General */
flow processing_calculate_flow(void *obj, region r)
{
	flow f; f.x = 0; f.y = 0;

	SharedCamera user_data = *((SharedCamera*) obj);
	camera = user_data->getFrame();
	cvtColor(camera, working1, COLOR_RGB2GRAY);

	if (previous.empty()) {
		working1.copyTo(previous);
		return f;
	}

	vector<Point2f> points_now, points_previous;
	vector<uchar> status;
	vector<float> err;

	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
	Size winSize(31, 31);

	working2.create(previous.size(), CV_8UC1);
	working2.setTo(0);

	rectangle(working2, Rect(r.x, r.y, r.w, r.h), Scalar(255), -1);

	goodFeaturesToTrack(previous, points_previous, MAX_COUNT, 0.01, 10, working2, 3, false, 0.04);

	if (points_previous.size() == 0) {
		return f;
	}

	calcOpticalFlowPyrLK(previous, working1, points_previous, points_now, status, err, winSize, 3, termcrit, 0, 0.001);

	int n = 0;
	float x = 0, y = 0;

	for (int i = 0; i < points_now.size(); i++) {
		if (!status[i]) continue;
		x+= points_now[i].x - points_previous[i].x;
		y+= points_now[i].y - points_previous[i].y;
		n++;
	}

	f.x = MAX(MIN(x / n, 100), -100);
	f.y = MAX(MIN(y / n, 100), -100);

	working1.copyTo(previous);

	return f;

}
