#include <ros/ros.h>

#include <std_msgs/String.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>
#include <ctime>
#include <cstdio>


using namespace std;
using namespace cv;
using namespace boost::filesystem;

//Takes depth image and saves it

Mat im;



void fileWriter(int nummPics, string extension, string folderMoniker, string workspacePath){
        string filename="KinectPicture";
        string truefile=filename+to_string(nummPics)+extension;
	string filePathInit=workspacePath+truefile;
	string filePathFinal=workspacePath+folderMoniker+"/"+truefile;
        imwrite(truefile, im);
	namedWindow(truefile, CV_WINDOW_AUTOSIZE);
	imshow(truefile, im);
	const char *first =filePathInit.c_str();
	const char *last =filePathFinal.c_str();
	
	int result;
	result= std::rename(first, last);
	if (result==0)
		puts ( "File successfully saved");
	
	else 
		perror( "Error saving file" );
	
       // cout << truefile<< " saved to the workspace"<<endl;
}

void UILoop(const ros::TimerEvent&){
        int numPics=0;
        bool doneWithPics=false;
        string answer;
	string ext= ".png";
	time_t t = time(NULL);
	tm * timePtr = localtime(&t);
	string timeName = to_string(timePtr->tm_year + 1900)+"-"+to_string(timePtr->tm_mon)+"-"+to_string(timePtr->tm_mday)+"-"+to_string(timePtr->tm_hour)+"-"+to_string(timePtr->tm_min)+"-"\
+to_string(timePtr->tm_sec);
	cout<<timeName<<endl;
	string folderName = "kinectpics" + timeName;
	path p{folderName};
	if(create_directories(p)){
		cout<<"Folder Created"<<endl;
	}
	string toWorkspace = "/home/trevorros/trevor_ws/";
	while(doneWithPics==false){
                cout << "Welcome to the Kinect Photo Booth!" <<endl<<endl;
                cout << "Take Photo? (y or n)"<<endl<<endl;
                cin >> answer;
		cout << endl;
        	if (answer=="y"){
			//Code for taking image out of stream here
			++numPics;
			fileWriter(numPics, ext, folderName, toWorkspace);
			while(doneWithPics==false){
                	cout << "Want to take another? (y or n)" << endl<<endl;
                	cin>>answer;
				if(answer=="y"){
                        		cout <<endl;
                        		++numPics;
					fileWriter(numPics, ext, folderName, toWorkspace);	
                		}
                		else if (answer=="n")
				{
					cout<<"Kinect Photo Booth Shutting Off..."<<endl;
                        		doneWithPics=true;
                        		ros::shutdown();
                		}
                		else{
                        		cout<<"Please answer y or n"<<endl;
                		}
        		}
		}
        	else if(answer=="n"){
                	cout<<"Kinect Photo Booth Shutting Off..."<<endl;
                	doneWithPics=true;
                	ros::shutdown();
        	}
        	else{
                	cout<<"Please answer y or n"<<endl;
        	}
	}
}




void imageCallback(const sensor_msgs::ImageConstPtr& msg){
	try
	{
		im = cv_bridge::toCvShare(msg, "bgr8")->image;
		imshow("view", im);
		waitKey(30);
	}
	catch(cv_bridge::Exception& e)
	{
		ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
	}
}



int main(int argc, char **argv)
{
	string rostopic;
	ros::init(argc, argv, "sampletextmaker");
  
	ros::NodeHandle n;
	namedWindow("view");
	startWindowThread();
	image_transport::ImageTransport it(n);
	cout<<"Enter RosTopic Name Below: "<<endl<<"Note: RosTopic must publish a /sensor_msgs/Image to be valid"<<endl<<endl;
	cin>>rostopic;
	image_transport::Subscriber sub = it.subscribe(rostopic, 1, imageCallback);
	ros::Timer timer = n.createTimer(ros::Duration(0.1), UILoop);
	ros::AsyncSpinner spinner(2);
	spinner.start();
	ros::waitForShutdown();
	destroyWindow("view");
}




/*Goals for this node:
This node is part UI part functionality
Basically it needs to ask the user if it wants to take a picture with the kinect
Then it uses OpenCV to display the image and saves it to a cache
Inside said cache are the image files and when the user wants to export them they can ask the UI to do this for them after displaying each image
I have to decide whether the user exports each image or exports them all in a giant for loop


1.Install OpenCV and add Header files (Done)
2.Figure out how to view images from the kinect
3.Figure out what type an image is on kinect
4.Design a better and prettier UI
5.Figure out how to export a file to the desktop or something idk 
6.Combine the two together to create the program


*/  
