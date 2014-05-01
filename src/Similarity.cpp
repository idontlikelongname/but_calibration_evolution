/*
 * Similarity.cpp
 *
 *  Created on: 8.1.2014
 *      Author: ivelas
 */

#include "Similarity.h"

using namespace std;
using namespace cv;

void Similarity2D::computeEntropies()
{
  vector<float> histogram_X(INTENSITIES, 0);
  vector<float> histogram_Y(INTENSITIES, 0);
  map<pair<uchar, uchar>, float> joint_histogram;

  for (int row = 0; row < X.rows; row++)
  {
    for (int col = 0; col < X.cols; col++)
    {
      uchar x_val = X.at<uchar>(row, col);
      uchar y_val = Y.at<uchar>(row, col);

      histogram_X[x_val] += 1;
      histogram_Y[y_val] += 1;

      joint_histogram[pair<uchar, uchar>(x_val, y_val)] += 1;
    }
  }

  float p;
  float points_nm = X.rows * X.cols;
  H_X = H_Y = H_XY = 0;
  for (int i = 0; i < INTENSITIES; i++)
  {
    p = histogram_X[i] / points_nm;
    if (p > 0)
    {
      H_X += -p * log(p);
    }

    p = histogram_Y[i] / points_nm;
    if (p > 0)
    {
      H_Y += -p * log(p);
    }
  }

  float points_nm_pow = points_nm * points_nm;
  for (map<pair<uchar, uchar>, float>::iterator i = joint_histogram.begin(); i != joint_histogram.end(); i++)
  {
    p = i->second / points_nm_pow;
    H_XY = -p * log(p);
  }
}

float SimilarityCameraLidar::projectionError(bool verbose)
{

  cv::Rect frame(cv::Point(0, 0), img_segments.size());
  int total_miss = 0;
  int total = 0;
  for (int i = 0; i < 2; i++)
  {
    int fire = 0;
    int miss = 0;
    for (::pcl::PointCloud<Velodyne::Point>::iterator pt = scan_segments[i].begin();
        pt < scan_segments[i].end(); pt++)
    {
      cv::Point xy = Velodyne::Velodyne::project(*pt, P);

      if (pt->z > 0 && xy.inside(frame))
      {
        if (img_segments.at<uchar>(xy) == i)
        {
          fire++;
        }
        else
        {
          miss++;
        }
        total++;
      }
    }
    total_miss += miss;
    if (verbose)
    {
      cout << "segment: " << i << ";\t ok: " << fire << ";\t missed: " << miss << endl;
    }
  }

  return total_miss / (float)(scan_segments[0].size() + scan_segments[1].size());
}

float SimilarityCameraLidar::edgeSimilarity()
{
  cv::Rect frame(cv::Point(0, 0), img.size());
  float CC = 0;
  for (::pcl::PointCloud<Velodyne::Point>::iterator pt = scan.begin(); pt < scan.end(); pt++)
  {
    cv::Point xy = Velodyne::Velodyne::project(*pt, P);

    if (pt->z > 0 && xy.inside(frame))
    {
      CC += img.at(xy) * pt->intensity;
    }
  }
  return CC;
}


