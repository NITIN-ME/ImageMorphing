#include<opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

/*
    checks if the distance
    between point first,
    and point second is less than one in
    both x and y
    axes individually.
*/

bool lies(Point2f &first, Point2f &second);

/*
  This code checks if the point lies inside area or not
  and returns the corresponding
  boolean value.
*/

bool inside(Rect &rect,const Point& pt) ;

Point2f minusgive(Point2f &p1, Rect &p2);

/*
  This function takes the images and
  does the warping and affine transformation
  on the images using the opencv affine gettransform and
  warpaffine function.
*/
void morphingshapes(Mat& im1,Mat& im2,Mat& img,vector<Point2f>& t1,vector<Point2f>& t2,vector<Point2f>& t,double blendfactor);

/*
 Driver program main function this
 asks for the iamge 1 image 2 name
 and their correspinding feature
  matching file containing the tie points in
  x,y tuple for separated by space and newline respectively.
*/

int main(){
    namedWindow("Morphed Face", WINDOW_AUTOSIZE);

    /*
        blending factor drives the
        program and show the gif thing.
    */
    double blendfactor = 0;
    // run until image 2 is produced.
    while(blendfactor <= 0.99){

    double secondblendfactor = 1 - (double)blendfactor;

    string image1("one.jpg");
    string image2("two.jpg");

    Mat im1 = imread(image1,CV_32F);
    Mat im2 = imread(image2,CV_32F);
    //earlier bug (fixed.)

    //////////////////////////////////
    im1.assignTo(im1, CV_32F);
    im2.assignTo(im2, CV_32F);
    /////////////////////////////////

    Mat imgMorph(im1.size(), CV_32FC3);

    vector<Point2f> points1;
    vector<Point2f> points2;
    vector<Point2f> points;

    /*
      Tie point containing files.
    */

    string f1 = "one.txt";
    string f2 = "two.txt";


    ifstream f(f1.c_str());
    float num1, num2;
    while(f >> num1 >> num2){
        points1.push_back(Point2f(num1,num2));
    }

    ifstream g(f2.c_str());
    while(g >> num1 >> num2){
        points2.push_back(Point2f(num1,num2));
    }

    Size size = im1.size();
    int w1 = size.width;
    int h1 = size.height;
    Rect rect(0, 0, w1, h1);
    /*
        Subdivisions used to generate the
        regions to find the delanuay
        triangulation points.
    */

    Subdiv2D subdiv1(rect);
    Subdiv2D subdiv2(rect);
    Subdiv2D subdiv3(rect);

    /*
        inserting points in the subdivision to
        generate the delanuay triangulation
    */


    for(int z = 0; z < points1.size(); z++){
          subdiv1.insert(points1[z]);
    }


    /*
        inserting points in the subdivision to
        generate the delanuay triangulation
    */

    for(int z = 0; z < points2.size(); z++){
          subdiv2.insert(points2[z]);
    }

    /*
        inserting points in the subdivision
        after mixing the images
        using the blendfactor in order to
        generate the trianglelist.
    */


    for(int i = 0; i < points1.size(); i++){
        float indexx, indexy;
        indexx = secondblendfactor*points1[i].x+blendfactor*points2[i].x;
        indexy = secondblendfactor*points1[i].y+blendfactor*points2[i].y;
        points.push_back(Point2f(indexx,indexy));
        subdiv3.insert(points[i]);
    }

    /*
        Using subdiv2D class to get
        the trianglelist.
        (fast approach!)
     */

    vector<Vec6f> triangleList1;
    vector<Vec6f> triangleList2;

    subdiv1.getTriangleList(triangleList1);
    subdiv2.getTriangleList(triangleList2);

    //cout<<triangleList1.size()<<"  "<<triangleList2.size()<<"   "<<triangleList3.size()<<endl;

    //ofstream tri("trinew.txt");
    vector<Point2f> pt(3);
    vector<int> found(3);
    vector<Point3i> arr3;

    for( size_t z = 0; z < triangleList1.size(); z++ ){
      Vec6f t = triangleList1[z];
      pt[0] = Point2f(t[0], t[1]);
      pt[1] = Point2f(t[2], t[3]);
      pt[2] = Point2f(t[4], t[5]);



  /*
      using the function created above to check
       if the point is inside the rectangle
       using the topleft point and width height property.
  */

    if(inside(rect, pt[0]) && inside(rect, pt[1]) && inside(rect, pt[2])){
        for(int j = 0; j < 3; j++)
          for(size_t k = 0; k < points.size(); k++)
            if(lies(pt[j],points1[k]))
                found[j] = k;

                /*
                    Using the rectangle for points got this
                    idea while learning from learnopencv online.
                */
                //tri<<ind[0]<<" "<<ind[1]<<" "<<ind[2]<<endl;

                //cout<<ind[0]<<" "<<ind[1]<<" "<<ind[2]<<endl;
                arr3.push_back(Point3i(found[0],found[1],found[2]));
              //cout<<arr3[i].x<<"  "<<arr3[i].y<<"  "<<arr3[i].z<<"  "<<endl;

      }
    }

  //  cout<<"================================================"<<endl;

    /*
      Uncomment this part to check for
      the points value.
    */
    ///////////////////////////////////////////////////////
    /*
    for(vector<Point3i>::iterator it = arr3.begin(); it <arr3.end(); it++){
      cout<<*it<<endl;
    }
    */
    ///////////////////////////////////////////////////////
    int x,y,z;
    //tri.close();

    ifstream ifs("trinew.txt");

    /*
        iterating to find
        the index number
        of the points that
        are forming the triangles.
    */

    for(vector<Point3i>::iterator it = arr3.begin(); it <arr3.end(); it++){
    //  cout<<*it<<endl;
      vector<Point2f> fs, sn, ls;

      //entering inside the first vector.
      fs.push_back( points1[(*it).x] );
      fs.push_back( points1[(*it).y] );
      fs.push_back( points1[(*it).z] );

    //entering inside the second vector.
      sn.push_back( points2[(*it).x] );
      sn.push_back( points2[(*it).y] );
      sn.push_back( points2[(*it).z] );

      //entering inside the third vector.
      ls.push_back( points[(*it).x] );
      ls.push_back( points[(*it).y] );
      ls.push_back( points[(*it).z] );

      morphingshapes(im1, im2, imgMorph, fs, sn, ls, blendfactor);

    }

    imshow("Morphed Face", imgMorph/255.0);

    waitKey(1);
    /*
        can change the blendfactor
         to change the number of
         transition states between
         original image and the final
         destination image.
         can be taken as user input also.
     */
    blendfactor += 0.03;
  }
    waitKey(1000);
    return(0);
}



bool lies(Point2f &first, Point2f &second){

  if(abs(first.x - second.x) < 1 && abs(first.y - second.y) < 1){
      return true;
  }

  else{
      return false;
  }

}



bool inside(Rect &rect,const Point& pt) {

    if(rect.x <= pt.x && pt.x < rect.x + rect.width && rect.y <= pt.y && pt.y < rect.y + rect.height){
        return true;
    }

    else{
        return false;
    }

}



void morphingshapes(Mat& im1,Mat& im2,Mat& img,vector<Point2f>& t1,vector<Point2f>& t2,vector<Point2f>& t,double blendfactor){

    Rect r = boundingRect(t);

    int wid = r.width;
    int hei = r.height;

    Rect r1 = boundingRect(t1);
    Rect r2 = boundingRect(t2);

    /*
        spacing the points by top-left
         corner of respective rects.
    */
    vector<Point2f> boundone, boundtwo, bounddef;
    vector<Point> tRectInt;

    Scalar sc(1,1,1);
    Scalar zero(0,0,0);

    double secondblendfactor = 1 - (double)blendfactor;

    int count = 0;
    while(count<3){
        bounddef.push_back(minusgive(t[count], r));
        tRectInt.push_back(minusgive(t[count], r));

        boundone.push_back(minusgive(t1[count], r1));
        boundtwo.push_back(minusgive(t2[count], r2));
        count++;
    }

    /*
          part set by filling the corresponding
         triangle susing the fillconvexpoly function.
    */

    Mat part = Mat(hei, wid, CV_32FC3,zero);
    fillConvexPoly(part, tRectInt, sc);
    Scalar one(1.0,1.0,1.0);

    Mat im1Rect, im2Rect;
    im1(r1).copyTo(im1Rect);
    im2(r2).copyTo(im2Rect);

    //cout<<"im1_  : "<<im1Rect.type()<<"   "<<CV_32F<<"   "<<im1.type()<<endl;

    Mat warpImage1(hei, wid, im1.type());
    Mat warpImage2(hei, wid, im2.type());

    Mat m1 = getAffineTransform(boundone, bounddef);
    Size sz1(warpImage1.cols,warpImage1.rows);
    warpAffine( im1Rect, warpImage1 , m1, sz1, INTER_NEAREST, BORDER_REFLECT);

    Mat m2 = getAffineTransform(boundtwo, bounddef);
    Size sz2(warpImage2.cols,warpImage2.rows);
    warpAffine( im2Rect, warpImage2 , m2, sz2, INTER_NEAREST, BORDER_REFLECT);


    Mat imgRect = secondblendfactor*warpImage1+blendfactor*warpImage2;

    multiply(imgRect,part, imgRect);
    multiply(img(r), one - part, img(r));
    img(r) = img(r) + imgRect;

}



Point2f minusgive(Point2f &p1, Rect &p2){
  float x = p1.x - p2.x;
  float y = p1.y - p2.y;
  return Point(x,y);
}
