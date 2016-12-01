// T_Server.cpp : Defines the entry point for the console application.
//

#include "T_Server.h"


void T_Server::processEvents(int size, int history, float x, float y)
{
	if (size > 0 && history < 1) {
		mousePressed(x, y);
	}
	else if (size && history) {
		mouseDragged(x, y);
	}
	else if (size < 1 && history > 0) {
		mouseReleased(x, y);
	}
}

void T_Server::mousePressed(float x, float y) {
	printf("pressed %f %f\n", x, y);

	TuioCursor *match = NULL;
	float distance = 0.01f;
	for (std::list<TuioCursor*>::iterator iter = stickyCursorList.begin(); iter != stickyCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x, y);
		if ((test < distance) && (test < 8.0f / width)) {
			distance = test;
			match = tcur;
		}
	}

	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

		if (match != NULL) {
			std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), match);
			if (joint != jointCursorList.end()) {
				jointCursorList.erase(joint);
			}
			stickyCursorList.remove(match);
			activeCursorList.remove(match);
			tuioServer->removeTuioCursor(match);
		}
		else {
			TuioCursor *cursor = tuioServer->addTuioCursor(x, y);
			stickyCursorList.push_back(cursor);
			activeCursorList.push_back(cursor);
		}
	}
	else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

		if (match != NULL) {
			std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), match);
			if (joint != jointCursorList.end()) {
				jointCursorList.remove(match);
			}
			else jointCursorList.push_back(match);
		}
	}
	else {
		if (match == NULL) {
			TuioCursor *cursor = tuioServer->addTuioCursor(x, y);
			activeCursorList.push_back(cursor);
		}
		else activeCursorList.push_back(match);
	}

	/*TuioCursor *cursor = tuioServer->addTuioCursor(x, y);
	activeCursorList.push_back(cursor);*/


	/*const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if ((keystate[SDL_SCANCODE_LSHIFT]) || (keystate[SDL_SCANCODE_RSHIFT])) {

	if (match != NULL) {
	std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), match);
	if (joint != jointCursorList.end()) {
	jointCursorList.erase(joint);
	}
	stickyCursorList.remove(match);
	activeCursorList.remove(match);
	tuioServer->removeTuioCursor(match);
	}
	else {
	TuioCursor *cursor = tuioServer->addTuioCursor(x, y);
	stickyCursorList.push_back(cursor);
	activeCursorList.push_back(cursor);
	}
	}
	else if ((keystate[SDL_SCANCODE_LCTRL]) || (keystate[SDL_SCANCODE_RCTRL])) {

	if (match != NULL) {
	std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), match);
	if (joint != jointCursorList.end()) {
	jointCursorList.remove(match);
	}
	else jointCursorList.push_back(match);
	}
	}
	else {
	if (match == NULL) {
	TuioCursor *cursor = tuioServer->addTuioCursor(x, y);
	activeCursorList.push_back(cursor);
	}
	else activeCursorList.push_back(match);
	}*/
}

void T_Server::mouseDragged(float x, float y) {
	printf("dragged %f %f\n", x, y);

	TuioCursor *cursor = NULL;
	float distance = width;
	for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter != activeCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x, y);
		if (test<distance) {
			distance = test;
			cursor = tcur;
		}
	}

	if (cursor == NULL) return;
	if (cursor->getTuioTime() == frameTime) return;

	std::list<TuioCursor*>::iterator joint = std::find(jointCursorList.begin(), jointCursorList.end(), cursor);
	if (joint != jointCursorList.end()) {
		float dx = x - cursor->getX();
		float dy = y - cursor->getY();
		for (std::list<TuioCursor*>::iterator iter = jointCursorList.begin(); iter != jointCursorList.end(); iter++) {
			TuioCursor *jointCursor = (*iter);
			tuioServer->updateTuioCursor(jointCursor, jointCursor->getX() + dx, jointCursor->getY() + dy);
		}
	}
	else tuioServer->updateTuioCursor(cursor, x, y);

	/*TuioCursor *cursor = NULL;
	float distance = width;
	for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter != activeCursorList.end(); iter++) {
	cursor = (*iter);
	if (cursor == NULL) return;
	if (cursor->getTuioTime() == frameTime) return;
	else tuioServer->updateTuioCursor(cursor, x, y);
	}*/
}

void T_Server::mouseReleased(float x, float y) {
	printf("released %f %f\n", x, y);

	TuioCursor *cursor = NULL;
	float distance = 0.01f;
	for (std::list<TuioCursor*>::iterator iter = stickyCursorList.begin(); iter != stickyCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x, y);
		if (test<distance) {
			distance = test;
			cursor = tcur;
		}
	}

	if (cursor != NULL) {
		activeCursorList.remove(cursor);
		return;
	}

	distance = 0.01f;
	for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter != activeCursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		float test = tcur->getDistance(x, y);
		if (test<distance) {
			distance = test;
			cursor = tcur;
		}
	}

	if (cursor != NULL) {
		activeCursorList.remove(cursor);
		tuioServer->removeTuioCursor(cursor);
	}

	/*TuioCursor *cursor = NULL;

	for (std::list<TuioCursor*>::iterator iter = activeCursorList.begin(); iter != activeCursorList.end(); iter++) {
	cursor = (*iter);
	}

	if (cursor != NULL) {
	activeCursorList.remove(cursor);
	tuioServer->removeTuioCursor(cursor);
	}*/
}

T_Server::T_Server(TuioServer *server)
	: verbose(false)
	, fullupdate(false)
	, periodic(false)
	, fullscreen(false)
	, help(false)
	, screen_width(1024)
	, screen_height(768)
	, window_width(640)
	, window_height(480)
{

	TuioTime::initSession();
	frameTime = TuioTime::getSessionTime();

	tuioServer = server;
	tuioServer->setSourceName("T_Server");
	tuioServer->enableObjectProfile(false);
	tuioServer->enableBlobProfile(false);

}

void T_Server::run(int size, int history, float x, float y) {

	frameTime = TuioTime::getSessionTime();
	tuioServer->initFrame(frameTime);
	processEvents(size, history, x, y);
	tuioServer->stopUntouchedMovingCursors();
	tuioServer->commitFrame();

	//cout << (size == history) << endl;

	//// simulate 50Hz compensating the previous processing time
	//int delay = 20 - (TuioTime::getSessionTime().getTotalMilliseconds() - frameTime.getTotalMilliseconds());
	//if (delay>0) SDL_Delay(delay);
}

Point2f getCalibrationPoint() {

	deque<int> xQueue;
	deque<int> yQueue;

	Point2f point;

	int xAvg = 0;
	int yAvg = 0;

	VideoCapture cap(1); // open the video file for reading

	if (!cap.isOpened()) // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	while (1)
	{
		Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		Mat image = frame;
		Mat new_image;

		image.convertTo(new_image, -1, alpha, beta);

		// Setup SimpleBlobDetector parameters.
		SimpleBlobDetector::Params params;

		// Change thresholds
		params.minThreshold = minThreshold;
		params.maxThreshold = maxThreshold;

		// Filter by Area.
		params.filterByArea = filterByArea;
		params.minArea = minArea;

		// Filter by Circularity
		params.filterByCircularity = filterByCircularity;
		params.minCircularity = minCircularity;

		// Filter by Convexity
		params.filterByConvexity = filterByConvexity;
		params.minConvexity = minConvexity;

		// Filter by Inertia
		params.filterByInertia = filterByInertia;
		params.minInertiaRatio = minInertiaRatio;

		//Filter by color
		params.filterByColor = filterByColor;
		params.blobColor = blobColor;

		//Create the simple blob detector
		//Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
		cv::SimpleBlobDetector detector(params);

		// Storage for blobs
		std::vector<KeyPoint> keypoints;

		// Detect blobs
		detector.detect(new_image, keypoints);

		///** Printing **/

		std::cout << "Points: " << keypoints.size() << std::endl;
		for (int i = 0; i < keypoints.size(); i++) {
			std::cout << "Point " << i << " x:" << keypoints[i].pt.x << " y:" << keypoints[i].pt.y << std::endl;
		}

		///** End Printing **/
		if (keypoints.size() != 1) {
			cout << "Too many or zero points detected. Fix!" << endl;
		}
		else {
			if (xQueue.size() == 10) {
				xQueue.pop_back();
				xQueue.push_front(keypoints[0].pt.x);
				yQueue.pop_back();
				yQueue.push_front(keypoints[0].pt.y);
			}
			else {
				xQueue.push_front(keypoints[0].pt.x);
				yQueue.push_front(keypoints[0].pt.y);
			}
		}

		for (int i = 0; i < xQueue.size(); i++) {
			xAvg += xQueue[i];
			yAvg += yQueue[i];
		}

		if (!xQueue.empty()) {
			xAvg = xAvg / (xQueue.size() + 1);
			yAvg = yAvg / (yQueue.size() + 1);

			cout << "xAvg: " << xAvg << endl;
			cout << "yAvg: " << yAvg << endl;
			cout << "Esc on a window to accept:" << endl;
		}
		else {
			cout << "No input yet." << endl;
		}

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob

		Mat im_with_keypoints;
		drawKeypoints(new_image, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		imshow("Original Video", frame); //show the frame in "Original Video" window
		imshow("Modified Video", new_image); //show the frame of which brightness increased
		imshow("Modified Video Keypoints", im_with_keypoints); //show the frame with keypoints

		if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	point.x = xAvg;
	point.y = yAvg;

	return point;
}

int main(int argc, char* argv[])
{
	/*	if (( argc != 1) && ( argc != 3)) {
	std::cout << "usage: T_Server [host] [port]\n";
	return 0;
	}*/

	//#ifndef __MACOSX__
	//	glutInit(&argc, argv);
	//#else
	//	if ((argc>1) && ((std::string(argv[1]).find("-NSDocumentRevisionsDebugMode") == 0) || (std::string(argv[1]).find("-psn_") == 0))) argc = 1;
	//#endif

	float lastX = 0;
	float lastY = 0;

	// Begin Server Setup-------------------------------------------------------------
	TuioServer *server = NULL;
	if (argc == 3) {
		server = new TuioServer(argv[1], atoi(argv[2]));
	}
	else server = new TuioServer(); // default is UDP port 3333 on localhost

									// add an additional TUIO/TCP sender
	OscSender *tcp_sender = NULL;
	if (argc == 2) {
		try { tcp_sender = new TcpSender(atoi(argv[1])); }
		catch (std::exception e) { tcp_sender = NULL; }
	}
	else if (argc == 3) {
		try { tcp_sender = new TcpSender(argv[1], atoi(argv[2])); }
		catch (std::exception e) { tcp_sender = NULL; }
	}
	else {
		try { tcp_sender = new TcpSender(3333); }
		catch (std::exception e) { tcp_sender = NULL; }
	}
	if (tcp_sender) server->addOscSender(tcp_sender);


	// add an additional TUIO/WEB sender
	OscSender *web_sender = NULL;
	try { web_sender = new WebSockSender(8080); }
	catch (std::exception e) { web_sender = NULL; }
	if (web_sender) server->addOscSender(web_sender);

	// add an additional TUIO/FLC sender
	OscSender *flash_sender = NULL;
	try { flash_sender = new FlashSender(); }
	catch (std::exception e) { flash_sender = NULL; }
	if (flash_sender) server->addOscSender(flash_sender);

	T_Server *app = new T_Server(server);

	// End Server Setup-------------------------------------------------------------

	int history = 0;

	/* Windows for displaying different frames */

	namedWindow("Original Video", CV_WINDOW_AUTOSIZE);
	namedWindow("Modified Video", CV_WINDOW_AUTOSIZE);
	namedWindow("Adjusted Modified Video", CV_WINDOW_AUTOSIZE);

	//namedWindow("Modified Video Keypoints", CV_WINDOW_AUTOSIZE);
	namedWindow("Adjusted Modified Video Keypoints", CV_WINDOW_AUTOSIZE);

	/* Calibration */


	//Uncomment
	Point2f topLeft;
	Point2f topRight;
	Point2f bottomRight;
	Point2f bottomLeft;

	cout << "Press enter to begin calibrating TL" << std::endl;
	cin.get();
	topLeft = getCalibrationPoint();
	cout << "Top Left Xcoor: " << topLeft.x << " Ycoor: " << topLeft.y << endl;

	cout << "Press enter to begin calibrating TR" << std::endl;
	cin.get();
	topRight = getCalibrationPoint();

	cout << "Press enter to begin calibrating BR" << std::endl;
	cin.get();
	bottomRight = getCalibrationPoint();

	cout << "Press enter to begin calibrating BL" << std::endl;
	cin.get();
	bottomLeft = getCalibrationPoint();

	VideoCapture cap(1); // open the video file for reading

	if (!cap.isOpened()) // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	Mat calibFrame;
	bool bSuccess = cap.read(calibFrame); // read a new frame from video
	if (!bSuccess) //if not success, break loop
	{
		cout << "Cannot read the frame from video file" << endl;
	}

	int height = calibFrame.rows;
	int width = calibFrame.cols;

	vector<Point2f> corners_fixed, corners_display;

	//Create corners vector of the fixed image
	corners_fixed.push_back(Point2f(0, 0));
	corners_fixed.push_back(Point2f(width, 0));
	corners_fixed.push_back(Point2f(width, height));
	corners_fixed.push_back(Point2f(0, height));

	//-----Start Calibration
	//Create corners vector of the display within the fixed image
	corners_display.push_back(topLeft);
	corners_display.push_back(topRight);
	corners_display.push_back(bottomRight);
	corners_display.push_back(bottomLeft);

	/*corners_display.push_back(Point2f(42, 59));
	corners_display.push_back(Point2f(629, 90));
	corners_display.push_back(Point2f(570, 408));
	corners_display.push_back(Point2f(59, 375));*/

	//-----End Caibration

	cout << "Calculating adjusment matrix.." << endl;

	//Create adjustment matrix
	Mat_<float> H_display_to_fixed = cv::getPerspectiveTransform(corners_fixed, corners_display);

	cv::Mat_<float> map1_32f(height, width), map2_32f(height, width);
	for (int y = 0; y<height; ++y)
	{
		float *buff_mapx = ((float*)map1_32f.data) + y*width;
		float *buff_mapy = ((float*)map2_32f.data) + y*width;
		for (int x = 0; x<width; ++x)
		{
			cv::Mat_<float> pt(3, 1);
			pt(0) = x;
			pt(1) = y;
			pt(2) = 1;
			pt = H_display_to_fixed*pt;
			pt /= pt(2);
			buff_mapx[x] = pt(0);
			buff_mapy[x] = pt(1);
		}
	}

	Mat map1_16u, map2_16u;
	convertMaps(map1_32f, map2_32f, map1_16u, map2_16u, CV_16SC2);
	// Keep map1_16u & map2_16u, discard the rest

	while (1)
	{
		Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		Mat image = frame;
		Mat new_image;

		double alpha = 1.0; // Contrast control
		double beta = 30.0; // Brightness control

		image.convertTo(new_image, -1, alpha, beta);

		// Setup SimpleBlobDetector parameters.
		SimpleBlobDetector::Params params;

		// Change thresholds
		params.minThreshold = minThreshold;
		params.maxThreshold = maxThreshold;

		// Filter by Area.
		params.filterByArea = filterByArea;
		params.minArea = minArea;

		// Filter by Circularity
		params.filterByCircularity = filterByCircularity;
		params.minCircularity = minCircularity;

		// Filter by Convexity
		params.filterByConvexity = filterByConvexity;
		params.minConvexity = minConvexity;

		// Filter by Inertia
		params.filterByInertia = filterByInertia;
		params.minInertiaRatio = minInertiaRatio;

		//Filter by color
		params.filterByColor = filterByColor;
		params.blobColor = blobColor;

		//Create the simple blob detector
		//Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
		cv::SimpleBlobDetector detector(params);

		// Storage for blobs
		std::vector<KeyPoint> keypoints;

		//Adjust image so display occupies entire frame
		Mat adj_new_image;
		remap(new_image, adj_new_image, map1_16u, map2_16u, INTER_LINEAR);

		// Detect blobs
		detector.detect(adj_new_image, keypoints);

		///** Printing **/

		//Uncomment
		/*std::cout << "Points: " << keypoints.size() << std::endl;
		for (int i = 0; i < keypoints.size(); i++) {
		std::cout << "Point " << i << " x:" << keypoints[i].pt.x << " y:" << keypoints[i].pt.y << std::endl;
		}*/

		///** End Printing **/

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob

		Mat im_with_keypoints;
		drawKeypoints(new_image, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


		imshow("Original Video", image); //show the frame in "Original Video" window
		imshow("Modified Video", new_image); //show the frame of which brightness increased
		imshow("Adjusted Modified Video", adj_new_image); //show the frame of which brightness increased
		imshow("Adjusted Modified Video Keypoints", im_with_keypoints);//show the frame with keypoints

		if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}

		if (keypoints.size() > 0)
		{
			app->run(keypoints.size(), history, keypoints[0].pt.x / width, keypoints[0].pt.y / height); //Server runs
			lastX = keypoints[0].pt.x / width;
			lastY = keypoints[0].pt.y / height;
		}
		else if (keypoints.size() < 1 && history > 0)
		{
			app->run(keypoints.size(), history, lastX, lastY);
		}

		//TuioTime frameTime = TuioTime::getSessionTime();
		//app->tuioServer->initFrame(frameTime);

		//if (keypoints.size() > 0 && history < 1)
		//{
		//	app->Tcur = app->tuioServer->addTuioCursor(keypoints[0].pt.x, keypoints[0].pt.y);
		//}
		//else if (keypoints.size() > 0 && history > 0)
		//{
		//	app->tuioServer->updateTuioCursor(app->Tcur, keypoints[0].pt.x, keypoints[0].pt.y);
		//}
		//else if (keypoints.size() < 1 && history > 0)
		//{
		//	app->tuioServer->removeTuioCursor(app->Tcur);
		//}

		////app->tuioServer->stopUntouchedMovingCursors();
		//app->tuioServer->commitFrame();

		history = keypoints.size();
	}



	delete app;
	delete server;
	return 0;
}