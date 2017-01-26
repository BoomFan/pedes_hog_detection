
//
// a program that detects pedestrians in the input picture

//standard includes
#include <stdlib.h>
#include <stdio.h>
#include <vector>

//ROS Includes
#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>

//opencv includes
#include <cv_bridge/cv_bridge.h>  
#include <image_transport/image_transport.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <stdio.h>

using namespace cv;



class SubscribeAndPublish
{
public:
  SubscribeAndPublish()
  {
    //Topic you want to publish
    pub1_ = n_.advertise<sensor_msgs::Image>("/camera/left/HOG_detection", 1);
    pub2_ = n_.advertise<sensor_msgs::Image>("/camera/right/HOG_detection", 1);
    //Topic you want to subscribe
    sub1_ = n_.subscribe("/camera/left/image_rect_color", 1, &SubscribeAndPublish::callbackleft, this);
    sub2_ = n_.subscribe("/camera/right/image_rect_color", 1, &SubscribeAndPublish::callbackright, this);
  }

  void callbackleft(const sensor_msgs::Image& input)
  {
    //.... do something with the input and generate the output...
    cv_bridge::CvImagePtr cv_ptr; // declare an instance of CvImage pointer    
        try  
        {  
            cv_ptr =  cv_bridge::toCvCopy(input, sensor_msgs::image_encodings::RGB8); //extract image from ROS msgs, create images in CV format and copy to CvImage pointer  
        }  
        catch(cv_bridge::Exception& e)  //show errors  
        {  
            ROS_ERROR("cv_bridge exception: %s", e.what());  
            return;  
        }  
    image_process(cv_ptr->image);
    sensor_msgs::ImagePtr output = cv_bridge::CvImage(std_msgs::Header(), "rgb8", cv_ptr->image).toImageMsg();
    pub1_.publish(output);
  }

  void callbackright(const sensor_msgs::Image& input)
  {
    //.... do something with the input and generate the output...
    cv_bridge::CvImagePtr cv_ptr; // declare an instance of CvImage pointer    
        try  
        {  
            cv_ptr =  cv_bridge::toCvCopy(input, sensor_msgs::image_encodings::RGB8); //extract image from ROS msgs, create images in CV format and copy to CvImage pointer  
        }  
        catch(cv_bridge::Exception& e)  //show errors  
        {  
            ROS_ERROR("cv_bridge exception: %s", e.what());  
            return;  
        }  
    image_process(cv_ptr->image);
    sensor_msgs::ImagePtr output = cv_bridge::CvImage(std_msgs::Header(), "rgb8", cv_ptr->image).toImageMsg();
    pub2_.publish(output);
  }

  void image_process(cv::Mat& img)   
  {
    vector<Rect> found;
    HOGDescriptor defaultHog;
    defaultHog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    //runing detection
    defaultHog.detectMultiScale(img, found);
    //draw rectangles and surround pedestrian
    for(int i = 0; i < found.size(); i++)
            {
        Rect r = found[i];
        rectangle(img, r.tl(), r.br(), Scalar(255, 0, 0), 2);
    }
   //an example of rectangle function:
   // cvRectangle function parameters： picture， upleft point， downright point， color， thickness， line type，point type
   //rectangle( img, cvPoint(5, 10), cvPoint(30, 40), cvScalar(201, 102, 62), 3, 4, 0 );
   cv::waitKey(2); 
  }  

private:
  ros::NodeHandle n_; 
  ros::Publisher pub1_;
  ros::Publisher pub2_;
  ros::Subscriber sub1_;
  ros::Subscriber sub2_;

};//End of class SubscribeAndPublish

int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "subscribe_and_publish");

  //Create an object of class SubscribeAndPublish that will take care of everything
  SubscribeAndPublish SAPObject;

  ros::spin();

  return 0;
}
